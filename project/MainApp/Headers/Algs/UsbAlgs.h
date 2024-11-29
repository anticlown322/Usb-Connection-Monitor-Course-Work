#ifndef USB_ALGS
#define USB_ALGS

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

