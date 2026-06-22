#pragma once

#include "IUserService.hpp"
#include "IUserRepository.hpp"
#include "IResourceRepository.hpp"

#include <memory>
#include <vector>

class UserService : public IUserService {
public:
    UserService(
        std::shared_ptr<IUserRepository>     userRepository,
        std::shared_ptr<IResourceRepository> resourceRepository
    );

    CreatedUserResult createUser(const CreateUserRequest& request)                   override;
    std::vector<User> getAllUsers()                                                   override;
    void              deactivateUser(int userId)                                     override;
    void              reactivateUser(int userId)                                     override;
    void              resetPassword(int userId, const ResetPasswordRequest& request) override;

private:
    std::shared_ptr<IUserRepository>     userRepository;
    std::shared_ptr<IResourceRepository> resourceRepository;

    static const std::vector<std::string> VALID_ROLES;

    void validateRole(const std::string& role) const;
};
