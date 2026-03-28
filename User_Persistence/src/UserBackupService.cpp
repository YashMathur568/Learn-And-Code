#include "UserBackupService.h"
#include <iostream>

std::string UserBackupService::createBackupFilePath(const User& user) const
{
    return "/backup/users/" + user.id + ".txt";
}

void UserBackupService::backup(const User& user) const
{
    std::string filePath = createBackupFilePath(user);
    std::cout << "Writing user backup to file: " << filePath << std::endl;
}
