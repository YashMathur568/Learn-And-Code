#include "Record.h"

Record::Record(const std::string& id,
               const std::string& name,
               double value,
               const std::string& date)
    : id(id),
      name(name),
      value(value),
      date(date),
      doubledValue(0),
      squaredValue(0)
{
}

const std::string& Record::getId() const { return id; }
const std::string& Record::getName() const { return name; }
double Record::getValue() const { return value; }
const std::string& Record::getDate() const { return date; }

double Record::getDoubledValue() const { return doubledValue; }
double Record::getSquaredValue() const { return squaredValue; }

void Record::setName(const std::string& newName) { name = newName; }
void Record::setDate(const std::string& newDate) { date = newDate; }

void Record::setDerivedValues(double doubled, double squared)
{
    doubledValue = doubled;
    squaredValue = squared;
}
