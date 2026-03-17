#ifndef RECORD_H
#define RECORD_H

#include <string>

class Record
{
private:
    std::string id;
    std::string name;
    double value;
    std::string date;

    double doubledValue;
    double squaredValue;

public:
    Record(const std::string& id,
           const std::string& name,
           double value,
           const std::string& date);

    const std::string& getId() const;
    const std::string& getName() const;
    double getValue() const;
    const std::string& getDate() const;

    double getDoubledValue() const;
    double getSquaredValue() const;

    void setName(const std::string& newName);
    void setDate(const std::string& newDate);
    void setDerivedValues(double doubled, double squared);
};

#endif
