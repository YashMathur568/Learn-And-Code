#include "AllocationService.hpp"
#include "../utils/AppException.hpp"
#include "../utils/DateUtils.hpp"

AllocationService::AllocationService(
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<IEmployeeRepository>   employeeRepository,
    std::shared_ptr<IProjectRepository>    projectRepository
) : allocationRepository(std::move(allocationRepository)),
    employeeRepository(std::move(employeeRepository)),
    projectRepository(std::move(projectRepository)) {}

Allocation AllocationService::createAllocation(int managerUserId, const CreateAllocationRequest& request) {
    if (request.utilisation < 1 || request.utilisation > 100) {
        throw ValidationException("Utilisation must be between 1 and 100.");
    }
    if (!isValidIsoDate(request.fromDate)) {
        throw ValidationException("fromDate is not a valid date. Expected DD-MM-YYYY.");
    }
    if (!isValidIsoDate(request.toDate)) {
        throw ValidationException("toDate is not a valid date. Expected DD-MM-YYYY.");
    }
    if (request.fromDate >= request.toDate) {
        throw ValidationException("fromDate must be before toDate.");
    }

    auto project = projectRepository->findById(request.projectId);
    if (!project.has_value()) {
        throw NotFoundException("Project not found.");
    }
    if (project->managerId != managerUserId) {
        throw UnauthorizedException("You do not manage this project.");
    }
    if (project->endDate < todayIso()) {
        throw ValidationException("Cannot allocate to a project that has already ended.");
    }
    if (request.fromDate < project->startDate) {
        throw ValidationException(
            "fromDate cannot be before the project start date (" + project->startDate + ")."
        );
    }
    if (request.toDate > project->endDate) {
        throw ValidationException(
            "toDate cannot be after the project end date (" + project->endDate + ")."
        );
    }

    auto employee = employeeRepository->findById(request.userId);
    if (!employee.has_value()) {
        throw NotFoundException("Employee not found.");
    }
    if (!employee->isActive) {
        throw ValidationException("Cannot allocate an inactive employee.");
    }

    const int currentUtilisation = allocationRepository->getTotalActiveUtilisation(request.userId);
    if (currentUtilisation + request.utilisation > 100) {
        throw ValidationException(
            "Allocation would exceed 100% utilisation. Current: " +
            std::to_string(currentUtilisation) + "%, requested: " +
            std::to_string(request.utilisation) + "%."
        );
    }

    Allocation allocation;
    allocation.userId       = request.userId;
    allocation.projectId   = request.projectId;
    allocation.utilisation = request.utilisation;
    allocation.fromDate    = request.fromDate;
    allocation.toDate      = request.toDate;

    const int allocationId = allocationRepository->create(allocation);
    employeeRepository->setStatus(request.userId, "ALLOCATED");
    allocation.allocationId = allocationId;
    return allocation;
}

Allocation AllocationService::endAllocation(int allocationId, int managerUserId) {
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
    if (project->managerId != managerUserId) {
        throw UnauthorizedException("You do not manage this project.");
    }

    allocationRepository->end(allocationId);

    if (!employeeRepository->hasActiveAllocations(allocation->userId)) {
        employeeRepository->setStatus(allocation->userId, "BENCH");
    }

    allocation->isActive = false;
    return allocation.value();
}

std::vector<Allocation> AllocationService::getByUserId(int userId) {
    return allocationRepository->findByUserId(userId);
}

std::vector<Allocation> AllocationService::getActiveByUserId(int userId) {
    return allocationRepository->findActiveByUserId(userId);
}

std::vector<Allocation> AllocationService::getActiveByProjectId(int projectId) {
    return allocationRepository->findActiveByProjectId(projectId);
}

int AllocationService::getTotalUtilisation(int userId) {
    return allocationRepository->getTotalActiveUtilisation(userId);
}

