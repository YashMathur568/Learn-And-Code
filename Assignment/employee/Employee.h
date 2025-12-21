#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>

class Employee
{
public:
    Employee(int id, std::string name, std::string department);

    int getId() const;
    const std::string& getName() const;
    const std::string& getDepartment() const;

    bool isActive() const;
    void deactivate();

private:
    int id;
    std::string name;
    std::string department;
    bool active;
};

#endif
