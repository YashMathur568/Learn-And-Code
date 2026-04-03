#include <iostream>
#include <string>
#include "ATMDeviceController.h"
#include "DeviceLockedException.h"
#include "InsufficientFundsException.h"
#include "NetworkConnectionException.h"
#include "DeviceStatus.h"
#include "WifiStatus.h"

void performWithdrawal(IATMDevice& atmDevice,
                       const std::string& accountId,
                       double withdrawalAmount)
{
    try
    {
        atmDevice.withdraw(accountId, withdrawalAmount);
        std::cout << "Withdrawal of $" << withdrawalAmount
                  << " completed successfully for account " << accountId << "."
                  << std::endl;
    }
    catch (const DeviceLockedException& exception)
    {
        std::cerr << "[Device Locked]      " << exception.what() << std::endl;
    }
    catch (const NetworkConnectionException& exception)
    {
        std::cerr << "[Network Error]      " << exception.what() << std::endl;
    }
    catch (const InsufficientFundsException& exception)
    {
        std::cerr << "[Insufficient Funds] " << exception.what() << std::endl;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[System Error]       " << exception.what() << std::endl;
    }
}

int main()
{
    std::cout << "=== ATM Withdrawal Simulation ===" << std::endl << std::endl;

    std::cout << "Scenario 1: Successful withdrawal" << std::endl;
    ATMDeviceController activeController(1500.00, DeviceStatus::Active, WifiStatus::Connected);
    performWithdrawal(activeController, "ACC-1001", 500.00);

    std::cout << std::endl;

    std::cout << "Scenario 2: Device is suspended" << std::endl;
    ATMDeviceController lockedController(1500.00, DeviceStatus::Suspended, WifiStatus::Connected);
    performWithdrawal(lockedController, "ACC-1002", 200.00);

    std::cout << std::endl;

    std::cout << "Scenario 3: No network connection" << std::endl;
    ATMDeviceController offlineController(1500.00, DeviceStatus::Active, WifiStatus::Disconnected);
    performWithdrawal(offlineController, "ACC-1003", 200.00);

    std::cout << std::endl;

    std::cout << "Scenario 4: Insufficient funds" << std::endl;
    ATMDeviceController lowBalanceController(100.00, DeviceStatus::Active, WifiStatus::Connected);
    performWithdrawal(lowBalanceController, "ACC-1004", 500.00);

    return 0;
}
