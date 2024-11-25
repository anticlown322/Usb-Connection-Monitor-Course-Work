#ifndef USB_ALGS
#define USB_ALGS

enum DEV_INFO : int {
    DESC,
    MFG,
    SERVICE,

    DRIVER_FILE,
    DRIVER_VERSION,

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

    std::wstring driverFilename;
    std::wstring driverVersion;

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

std::wstring GetRegistryStringValue(HKEY hKey, const std::wstring& valueName);

std::wstring GetDeviceDescription(HKEY hKey);
std::wstring GetDeviceManufactoring(HKEY hKey);

bool IsDeviceConnected(HKEY hKey);
bool IsDeviceDisabled(HKEY hKey);
bool IsDeviceSafeToUnplug(HKEY hKey);
bool IsUsbHub(HKEY hKey); 
std::wstring GetDeviceCapabilities(HKEY hKey);

std::vector<USBDeviceInfo> ListUSBDevices(const std::wstring& registryPath);
std::vector<USBDeviceInfo> GetAllUsbDevices();

#endif // !USB_ALGS

