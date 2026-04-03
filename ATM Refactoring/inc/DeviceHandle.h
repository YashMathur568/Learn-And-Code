#ifndef DEVICE_HANDLE_H
#define DEVICE_HANDLE_H

class DeviceHandle
{
public:
    explicit DeviceHandle(int assignedDeviceId)
        : assignedDeviceId(assignedDeviceId), handleIsValid(true) {}

    bool isValid() const { return handleIsValid; }
    int getAssignedDeviceId() const { return assignedDeviceId; }

private:
    int assignedDeviceId;
    bool handleIsValid;
};

#endif
