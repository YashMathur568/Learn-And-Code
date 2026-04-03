#ifndef ATM_DEVICE_CONTROLLER_H
#define ATM_DEVICE_CONTROLLER_H

#include <string>
#include "IATMDevice.h"
#include "DeviceHandle.h"
#include "DeviceRecord.h"
#include "DeviceId.h"
#include "DeviceStatus.h"
#include "WifiStatus.h"

class ATMDeviceController : public IATMDevice
{
public:
    ATMDeviceController(double accountBalance,
                        DeviceStatus deviceStatus,
                        WifiStatus wifiStatus);

    void withdraw(const std::string& accountId, double amount) override;

private:
    double simulatedAccountBalance;
    DeviceStatus simulatedDeviceStatus;
    WifiStatus simulatedWifiStatus;

    DeviceHandle acquireDeviceHandle(DeviceId deviceId);
    DeviceRecord retrieveDeviceRecord(const DeviceHandle& deviceHandle);

    void ensureDeviceIsActive(const DeviceRecord& deviceRecord);
    void ensureNetworkIsConnected(const DeviceRecord& deviceRecord);
    void ensureSufficientFunds(const std::string& accountId, double amount);

    double getAccountBalance(const std::string& accountId);
    void dispenseCash(const DeviceHandle& deviceHandle, double amount);
};

#endif
