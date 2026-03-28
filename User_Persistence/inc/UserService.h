#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "UserValidator.h"
#include "UserRepository.h"
#include "UserBackupService.h"

class UserService
{
public:
    void saveUser(const User& user) const;

private:
    UserValidator validator;
    UserRepository repository;
    UserBackupService backupService;
};

#endif
