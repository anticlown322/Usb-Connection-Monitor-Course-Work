#ifndef USB_ALGS
#define USB_ALGS

enum DEV_INFO : int {
    DESC,
    MFG,
    SERVICE,

    IS_CONNECTED,
    IS_DIASBLED,
    IS_SAFE_TO_UNPLUG,
    IS_USB_HUB,
    CAPABILITIES,

    LOCATION,

    SERIAL_NUM,
    USB_CLASS,
    USB_SUBCLASS,
    USB_PROTOCOL,

    ID,
    DEV_GUID,
    HARDWARE_ID,
    PARENT_PREFIX
};

struct USBDeviceInfo {
    std::wstring description;
    std::wstring manufactoring;
    std::wstring serviceName;

    bool isConnected;
    bool isDisabled;
    bool isSafeToUnplug;
    bool isUsbHub;
    std::wstring capabilities;

    std::wstring location;
    
    std::wstring serialNumber;
    std::wstring usbClass;
    std::wstring usbSubclass;
    std::wstring usbProtocol;
    
    std::wstring id;
    std::wstring devClassGUID;
    std::wstring hardwareID;
    std::wstring parentIDPrefix;
};

/* common */
std::wstring GetRegistryStringValue(HKEY hKey, const std::wstring& valueName);
DWORD GetRegistryDWORDValue(HKEY hKey, const std::wstring& valueName);
std::vector<USBDeviceInfo> ListUSBDevices(const std::wstring& registryPath);
std::vector<USBDeviceInfo> GetAllUsbDevices();

/* basic info */
std::wstring GetDeviceDescription(HKEY hKey);
std::wstring GetDeviceManufactoring(HKEY hKey);

/* status */
//void getDevicesStatus(std::vector<USBDeviceInfo> &devices);

/* capabilities */
std::wstring GetDeviceCapabilities(HKEY hKey);

/* manufactoring info */
std::wstring GetDeviceSerialNumber(HKEY hKey);
std::wstring ExtractUsbField(const std::wstring& compatibleIDs, const std::wregex& pattern);
std::wstring GetDeviceUsbClass(HKEY hKey);
std::wstring GetDeviceUsbSubclass(HKEY hKey);
std::wstring GetDeviceUsbProtocol(HKEY hKey);

#endif // !USB_ALGS

