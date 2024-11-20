#ifndef USB_ALGS
#define USB_ALGS

struct USBDeviceInfo {
    std::wstring deviceDescription;
    std::wstring deviceID;
    std::wstring hardwareID;
    std::wstring deviceClass;
    GUID deviceClassGUID;
    bool isConnected;
    bool isDisabled;
};

std::vector<USBDeviceInfo> getUSBDevicesInformation();
bool IsUSBDevice(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pDeviceInfoData);

#endif // !USB_ALGS

