#include "MainApp/Headers/App/general.h"
#include "MainApp/Headers/Algs/UsbAlgs.h"

std::wstring GetRegistryStringValue(HKEY hKey, const std::wstring& valueName) {
    DWORD type;
    wchar_t value[256];
    DWORD valueSize = sizeof(value);

    if (RegQueryValueExW(
        hKey, 
        valueName.c_str(),
        NULL, 
        &type, 
        (LPBYTE)value, 
        &valueSize) == ERROR_SUCCESS) 
    {
        return value;
    }

    return L"Unknown";
}

std::wstring GetDeviceDescription(HKEY hKey) 
{
    std::wstring desc = GetRegistryStringValue(hKey, L"FriendlyName");

    if (desc == L"Unknown")
    {
        desc = GetRegistryStringValue(hKey, L"DeviceDesc");
    }

    size_t pos = desc.rfind(L';');
    if (pos != std::wstring::npos && pos + 1 < desc.length()) {
        return desc.substr(pos + 1);
    }
    return L"Unknown";
};

std::wstring GetDeviceManufactoring(HKEY hKey)
{
    std::wstring manufactoring = GetRegistryStringValue(hKey, L"Mfg");
    size_t startMarker = manufactoring.find(L'%');
    size_t endMarker = manufactoring.find(L'%', startMarker + 1);

    if (startMarker != std::wstring::npos && endMarker != std::wstring::npos) 
    {
        size_t semicolonPos = manufactoring.find(L';', endMarker + 1);
        
        if (semicolonPos != std::wstring::npos) 
        {
            size_t bracketPos = manufactoring.find(L'(', endMarker + 1);

            if (bracketPos != std::wstring::npos)
            {
                endMarker = manufactoring.find(L')', bracketPos + 1);
                return manufactoring.substr(bracketPos + 1, endMarker - (bracketPos + 1));
            }

            return manufactoring.substr(semicolonPos + 1);
        }
    }

    return manufactoring;
};

bool IsDeviceConnected(HKEY hKey)
{
    std::wstring status = GetRegistryStringValue(hKey, L"ConfigFlags");
    return status != L"1";
}

bool IsDeviceDisabled(HKEY hKey)
{
    std::wstring status = GetRegistryStringValue(hKey, L"ConfigFlags");
    return status != L"1";
}

bool IsDeviceSafeToUnplug(HKEY hKey)
{
    std::wstring status = GetRegistryStringValue(hKey, L"ConfigFlags");
    return status != L"1";
}

bool IsUsbHub(HKEY hKey)
{
    std::wstring status = GetRegistryStringValue(hKey, L"ConfigFlags");
    return status != L"1";
}

std::wstring GetDeviceCapabilities(HKEY hKey)
{
    return L"";
}

std::vector<USBDeviceInfo> ListUSBDevices(const std::wstring& registryPath) 
{
    std::vector<USBDeviceInfo> devices;
    HKEY hKey;

    if (RegOpenKeyExW(
        HKEY_LOCAL_MACHINE, 
        registryPath.c_str(), 
        0, 
        KEY_READ, 
        &hKey) != ERROR_SUCCESS) 
    {
        std::cerr << "Error: Unable to open registry key." << std::endl;
        return devices;
    }

    DWORD index = 0;
    DWORD nameSize = 256;
    wchar_t name[256];

    while(RegEnumKeyExW(
        hKey, 
        index, 
        name, 
        &nameSize, 
        NULL, NULL, NULL, NULL) == ERROR_SUCCESS) 
    {
        std::wstring deviceKeyPath = registryPath + L"\\" + name;
        HKEY deviceKey;

        if (RegOpenKeyExW(hKey, name, 0, KEY_READ, &deviceKey) == ERROR_SUCCESS) 
        {
            DWORD subIndex = 0;
            DWORD subNameSize = 256;
            wchar_t subName[256];

            while (RegEnumKeyExW(
                deviceKey, 
                subIndex, 
                subName, 
                &subNameSize, 
                NULL, NULL, NULL, NULL) == ERROR_SUCCESS) 
            {
                std::wstring instanceKeyPath = deviceKeyPath + L"\\" + subName;
                HKEY instanceKey;

                if (RegOpenKeyExW(
                    deviceKey, 
                    subName, 
                    0, 
                    KEY_READ, 
                    &instanceKey) == ERROR_SUCCESS) 
                {
                    USBDeviceInfo deviceInfo;

                    /* basic info */
                    deviceInfo.description = GetDeviceDescription(instanceKey);
                    deviceInfo.manufactoring = GetDeviceManufactoring(instanceKey);
                    deviceInfo.serviceName = GetRegistryStringValue(instanceKey, L"Service");

                    /* drivers*/
                    deviceInfo.driverFilename = L"";
                    deviceInfo.driverVersion = L"";

                    /* capabilities */
                    deviceInfo.isConnected = IsDeviceConnected(instanceKey);
                    deviceInfo.isDisabled = IsDeviceDisabled(instanceKey);
                    deviceInfo.isSafeToUnplug = IsDeviceSafeToUnplug(instanceKey);
                    deviceInfo.isUsbHub = IsUsbHub(instanceKey);
                    deviceInfo.capabilities = GetDeviceCapabilities(instanceKey);

                    /* location */
                    deviceInfo.location = GetRegistryStringValue(instanceKey, L"LocationInformation");

                    /* manufactoring info */
                    deviceInfo.serialNumber = L"";
                    deviceInfo.usbClass = L"";
                    deviceInfo.usbSubclass = L"";
                    deviceInfo.usbProtocol = L"";

                    /* IDS*/
                    deviceInfo.id = std::wstring(name) + L"\\" + subName;
                    deviceInfo.hardwareID = GetRegistryStringValue(instanceKey, L"HardwareID");
                    deviceInfo.devClassGUID = GetRegistryStringValue(instanceKey, L"ClassGUID");
                    deviceInfo.parentIDPrefix = GetRegistryStringValue(instanceKey, L"ParentIdPrefix");
                    
                    devices.push_back(deviceInfo);
                    RegCloseKey(instanceKey);
                }

                subIndex++;
                subNameSize = 256;
            }

            RegCloseKey(deviceKey);
        }

        index++;
        nameSize = 256;
    }

    RegCloseKey(hKey);
    return devices;
}

std::vector<USBDeviceInfo> GetAllUsbDevices()
{
    std::vector<USBDeviceInfo> usbDevices = ListUSBDevices(L"SYSTEM\\CurrentControlSet\\Enum\\USB");
    std::vector<USBDeviceInfo> usbPrintDevices = ListUSBDevices(L"SYSTEM\\CurrentControlSet\\Enum\\USBPRINT");
    std::vector<USBDeviceInfo> usbStorDevices = ListUSBDevices(L"SYSTEM\\CurrentControlSet\\Enum\\USBSTOR");

    usbDevices.insert(usbDevices.end(), usbPrintDevices.begin(), usbPrintDevices.end());
    usbDevices.insert(usbDevices.end(), usbStorDevices.begin(), usbStorDevices.end());

    return usbDevices;
}