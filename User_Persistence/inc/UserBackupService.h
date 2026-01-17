#ifndef USER_BACKUP_SERVICE_H
#define USER_BACKUP_SERVICE_H

#include "User.h"
#include <string>

class UserBackupService
{
public:
    void backup(const User& user) const;

private:
    std::string createBackupFilePath(const User& user) const;
};

#endif
