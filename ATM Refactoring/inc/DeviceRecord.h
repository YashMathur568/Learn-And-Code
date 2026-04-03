#ifndef DEVICE_RECORD_H
#define DEVICE_RECORD_H

#include "DeviceStatus.h"
#include "WifiStatus.h"

class DeviceRecord
{
public:
    DeviceRecord(DeviceStatus status, WifiStatus wifiStatus)
        : deviceStatus(status), wifiStatus(wifiStatus) {}

    DeviceStatus getDeviceStatus() const { return deviceStatus; }
    WifiStatus getWifiStatus() const { return wifiStatus; }

private:
    DeviceStatus deviceStatus;
    WifiStatus wifiStatus;
};

#endif
