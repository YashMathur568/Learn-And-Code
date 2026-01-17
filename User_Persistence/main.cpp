#include "UserService.h"
#include "User.h"

int main()
{
    User user;
    user.id = "123";
    user.name = "Yash";
    user.email = "yash@example.com";

    UserService userService;
    userService.saveUser(user);

    return 0;
}
