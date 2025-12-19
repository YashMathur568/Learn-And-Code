#ifndef USER_H
#define USER_H

#include <string>

class User
{
public:
    User(const std::string& name, int age);

    std::string getName() const;
    int getAge() const;

private:
    std::string userName;
    int userAge;
};

#endif
