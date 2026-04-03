#include "ATMDeviceController.h"
#include "DeviceLockedException.h"
#include "InsufficientFundsException.h"
#include "NetworkConnectionException.h"
#include <iostream>
#include <sstream>
#include <iomanip>

ATMDeviceController::ATMDeviceController(double accountBalance,
                                         DeviceStatus deviceStatus,
                                         WifiStatus wifiStatus)
    : simulatedAccountBalance(accountBalance),
      simulatedDeviceStatus(deviceStatus),
      simulatedWifiStatus(wifiStatus)
{}

void ATMDeviceController::withdraw(const std::string& accountId, double amount)
{
    DeviceHandle deviceHandle = acquireDeviceHandle(DeviceId::DEV1);
    DeviceRecord deviceRecord = retrieveDeviceRecord(deviceHandle);

    ensureDeviceIsActive(deviceRecord);
    ensureNetworkIsConnected(deviceRecord);
    ensureSufficientFunds(accountId, amount);

    dispenseCash(deviceHandle, amount);
}

DeviceHandle ATMDeviceController::acquireDeviceHandle(DeviceId deviceId)
{
    return DeviceHandle(static_cast<int>(deviceId));
}

DeviceRecord ATMDeviceController::retrieveDeviceRecord(const DeviceHandle& deviceHandle)
{
    (void)deviceHandle;
    return DeviceRecord(simulatedDeviceStatus, simulatedWifiStatus);
}

void ATMDeviceController::ensureDeviceIsActive(const DeviceRecord& deviceRecord)
{
    if (deviceRecord.getDeviceStatus() == DeviceStatus::Suspended)
    {
        throw DeviceLockedException(
            "ATM device is currently suspended and cannot process transactions.");
    }
}

void ATMDeviceController::ensureNetworkIsConnected(const DeviceRecord& deviceRecord)
{
    if (deviceRecord.getWifiStatus() != WifiStatus::Connected)
    {
        throw NetworkConnectionException(
            "ATM device has no active network connection. Please try again later.");
    }
}

void ATMDeviceController::ensureSufficientFunds(const std::string& accountId, double amount)
{
    double availableBalance = getAccountBalance(accountId);

    if (availableBalance < amount)
    {
        std::ostringstream message;
        message << std::fixed << std::setprecision(2);
        message << "Insufficient funds: requested $" << amount
                << " but available balance is $" << availableBalance << ".";

        throw InsufficientFundsException(message.str());
    }
}

double ATMDeviceController::getAccountBalance(const std::string& accountId)
{
    (void)accountId;
    return simulatedAccountBalance;
}

void ATMDeviceController::dispenseCash(const DeviceHandle& deviceHandle, double amount)
{
    (void)deviceHandle;
    std::cout << "Dispensing $" << std::fixed << std::setprecision(2)
              << amount << " successfully." << std::endl;
}
