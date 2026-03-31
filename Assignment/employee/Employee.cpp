#include "Employee.h"

Employee::Employee(int id, std::string name, std::string department)
    : id(id),
      name(std::move(name)),
      department(std::move(department)),
      active(true)
{
}

int Employee::getId() const { return id; }
const std::string& Employee::getName() const { return name; }
const std::string& Employee::getDepartment() const { return department; }

bool Employee::isActive() const { return active; }

void Employee::deactivate()
{
    active = false;
}
