#include "AllocationService.hpp"
#include "../utils/AppException.hpp"

AllocationService::AllocationService(
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<IEmployeeRepository>   employeeRepository,
    std::shared_ptr<IProjectRepository>    projectRepository
) : allocationRepository(std::move(allocationRepository)),
    employeeRepository(std::move(employeeRepository)),
    projectRepository(std::move(projectRepository)) {}

Allocation AllocationService::createAllocation(int managerEmployeeId, const CreateAllocationRequest& request) {
    if (request.utilisation < 1 || request.utilisation > 100) {
        throw ValidationException("Utilisation must be between 1 and 100.");
    }
    if (request.fromDate >= request.toDate) {
        throw ValidationException("fromDate must be before toDate.");
    }

    auto project = projectRepository->findById(request.projectId);
    if (!project.has_value()) {
        throw NotFoundException("Project not found.");
    }
    if (project->managerId != managerEmployeeId) {
        throw UnauthorizedException("You do not manage this project.");
    }

    auto employee = employeeRepository->findById(request.employeeId);
    if (!employee.has_value()) {
        throw NotFoundException("Employee not found.");
    }
    if (!employee->isActive) {
        throw ValidationException("Cannot allocate an inactive employee.");
    }

    const int currentUtilisation = allocationRepository->getTotalActiveUtilisation(request.employeeId);
    if (currentUtilisation + request.utilisation > 100) {
        throw ValidationException(
            "Allocation would exceed 100% utilisation. Current: " +
            std::to_string(currentUtilisation) + "%, requested: " +
            std::to_string(request.utilisation) + "%."
        );
    }

    Allocation allocation;
    allocation.employeeId  = request.employeeId;
    allocation.projectId   = request.projectId;
    allocation.utilisation = request.utilisation;
    allocation.fromDate    = request.fromDate;
    allocation.toDate      = request.toDate;

    const int allocationId = allocationRepository->create(allocation);
    employeeRepository->setStatus(request.employeeId, "ALLOCATED");
    allocation.allocationId = allocationId;
    return allocation;
}

Allocation AllocationService::endAllocation(int allocationId, int managerEmployeeId) {
    auto allocation = allocationRepository->findById(allocationId);
    if (!allocation.has_value()) {
        throw NotFoundException("Allocation not found.");
    }
    if (!allocation->isActive) {
        throw ValidationException("Allocation is already ended.");
    }

    auto project = projectRepository->findById(allocation->projectId);
    if (!project.has_value()) {
        throw NotFoundException("Project not found.");
    }
    if (project->managerId != managerEmployeeId) {
        throw UnauthorizedException("You do not manage this project.");
    }

    allocationRepository->end(allocationId);

    if (!employeeRepository->hasActiveAllocations(allocation->employeeId)) {
        employeeRepository->setStatus(allocation->employeeId, "BENCH");
    }

    allocation->isActive = false;
    return allocation.value();
}

std::vector<Allocation> AllocationService::getByEmployeeId(int employeeId) {
    return allocationRepository->findByEmployeeId(employeeId);
}

std::vector<Allocation> AllocationService::getActiveByEmployeeId(int employeeId) {
    return allocationRepository->findActiveByEmployeeId(employeeId);
}

std::vector<Allocation> AllocationService::getActiveByProjectId(int projectId) {
    return allocationRepository->findActiveByProjectId(projectId);
}

int AllocationService::getTotalUtilisation(int employeeId) {
    return allocationRepository->getTotalActiveUtilisation(employeeId);
}
