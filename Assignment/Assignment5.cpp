// Why the Employee class does NOT follow SRP (Single Responsibility Principle)

// 1.Multiple responsibilities in one class

// ->Holds employee data (id, name, department)
// ->Manages business logic (terminateEmployee, isWorking)
// ->Handles persistence (saveEmployeeToDatabase)
// ->Handles presentation/reporting (XML & CSV reports)

// 2.More than one reason to change

// ->Database schema changes → modify Employee
// ->Report format changes → modify Employee
// ->Business rule changes (termination logic) → modify Employee

// 3.Mixes different layers

// ->Domain logic (employee state)
// ->Infrastructure (database saving)
// ->Presentation (report printing)

// 4.High coupling

// ->Employee depends on database and report formats

// 5.Hard to test

// ->Cannot test employee logic without database or report code


#include <string>
using namespace std;

class Employee
{
private:
    int id;
    string name;
    string department;
    bool working;

public:
    Employee(int id, string name, string department)
        : id(id), name(name), department(department), working(true) {}

    void terminate()
    {
        working = false;
    }

    bool isWorking() const
    {
        return working;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    string getDepartment() const { return department; }
};

// ---------- PERSISTENCE ----------

class EmployeeRepository
{
public:
    void save(const Employee& employee)
    {
        // Save employee to database
    }
};

// ---------- REPORTING ----------

class EmployeeReportPrinter
{
public:
    void printXmlReport(const Employee& employee)
    {
        // Print employee details in XML format
    }

    void printCsvReport(const Employee& employee)
    {
        // Print employee details in CSV format
    }
};
