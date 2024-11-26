#include "MainApp/Headers/App/general.h"
#include "MainApp/Headers/Algs/UsbAlgs.h"

/* basic info */
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
}

/* status */

void getDevicesStatus(std::vector<USBDeviceInfo> &devices)
{
    HDEVINFO deviceInfoSet;
    SP_DEVINFO_DATA deviceInfoData;
    DWORD index = 0;

    deviceInfoSet = SetupDiGetClassDevs(NULL, L"USB", NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) 
    {
        std::cerr << "Can't get USB devices list" << std::endl;
    }

    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    while (SetupDiEnumDeviceInfo(
        deviceInfoSet, 
        index, 
        &deviceInfoData)) 
    {
        index++;

        TCHAR deviceName[256];
        DWORD status, problemCode;

        for (int i = 0; i < devices.size(); i++)
        {
            GUID devGuid;
            HRESULT hResult = CLSIDFromString(devices[i].devClassGUID.c_str(), &devGuid);

            if(SUCCEEDED(hResult) && deviceInfoData.ClassGuid == devGuid)
                if (CM_Get_DevNode_Status(
                    &status,
                    &problemCode,
                    deviceInfoData.DevInst,
                    0) == CR_SUCCESS)
                {
                    devices[i].isConnected = !(status & DN_HAS_PROBLEM);
                    devices[i].isDisabled = status & DN_DRIVER_BLOCKED; 
                    devices[i].isSafeToUnplug = status & DN_REMOVABLE;
                }
                else
                {
                    devices[i].isConnected = false;
                    devices[i].isDisabled = false;
                    devices[i].isSafeToUnplug = true;
                }
        }
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}

/* status */

std::wstring GetDeviceCapabilities(HKEY hKey)
{
    const DWORD MASK_REMOVABLE = 0x04;
    const DWORD MASK_UNIQUE_ID = 0x10;
    const DWORD MASK_SILENT_INSTALL = 0x20;
    const DWORD MASK_RAW_DEVICE_OK = 0x40;
    const DWORD MASK_SURPRISE_REMOVAL_OK = 0x80;

    std::wstring capabilities = L"";
    DWORD capabilitiesDwordValue = GetRegistryDWORDValue(hKey, L"Capabilities");

    if (capabilitiesDwordValue & MASK_REMOVABLE)
        capabilities += L"Removable ";

    if (capabilitiesDwordValue & MASK_UNIQUE_ID)
        capabilities += L"UniqueID ";

    if (capabilitiesDwordValue & MASK_SILENT_INSTALL)
        capabilities += L"SilentInstall ";

    if (capabilitiesDwordValue & MASK_RAW_DEVICE_OK)
        capabilities += L"RawDeviceOK ";

    if (capabilitiesDwordValue & MASK_SURPRISE_REMOVAL_OK)
        capabilities += L"SurpriseRemovalOK ";

    if (capabilities.empty())
        capabilities = L"Unknown";

    return capabilities;
}

/* manufactoring info */
std::wstring GetDeviceSerialNumber(HKEY hKey)
{
    std::wstring name = L"Device Parameters";
    HKEY devParamsKey;

    if (RegOpenKeyExW(
        hKey,
        name.c_str(),
        0,
        KEY_READ,
        &devParamsKey) == ERROR_SUCCESS)
    {
        const std::wstring symbolicName = GetRegistryStringValue(devParamsKey, L"SymbolicName");
        std::wregex re(LR"(USB#VID_[0-9A-Fa-f]+&PID_[0-9A-Fa-f]+#([0-9A-Fa-f]+)#)");
        std::wsmatch match;

        if (std::regex_search(symbolicName, match, re) && match.size() > 1)
        {
            return match[1].str();
        }
    }

    return L"";
}

std::wstring ExtractUsbField(const std::wstring& compatibleIDs, const std::wregex& pattern)
{
    std::wsmatch match;
    if (std::regex_search(compatibleIDs, match, pattern) && match.size() > 1)
    {
        return match.str(1);
    }

    return L"00"; //if not found 
}

std::wstring GetDeviceUsbClass(HKEY hKey)
{
    std::wstring compatibleIDs = GetRegistryStringValue(hKey, L"CompatibleIDs");
    std::wregex classPattern(LR"(USB\\Class_([0-9A-Fa-f]{2}))");

    return ExtractUsbField(compatibleIDs, classPattern);
}

std::wstring GetDeviceUsbSubclass(HKEY hKey)
{
    std::wstring compatibleIDs = GetRegistryStringValue(hKey, L"CompatibleIDs");
    std::wregex subclassPattern(LR"(USB\\Class_[0-9A-Fa-f]{2}&SubClass_([0-9A-Fa-f]{2}))");

    return ExtractUsbField(compatibleIDs, subclassPattern);
}

std::wstring GetDeviceUsbProtocol(HKEY hKey)
{
    std::wstring compatibleIDs = GetRegistryStringValue(hKey, L"CompatibleIDs");
    std::wregex protocolPattern(LR"(USB\\Class_[0-9A-Fa-f]{2}&SubClass_[0-9A-Fa-f]{2}&Prot_([0-9A-Fa-f]{2}))");

    return ExtractUsbField(compatibleIDs, protocolPattern);
}

/* common */
std::wstring GetRegistryStringValue(HKEY hKey, const std::wstring& valueName)
{
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

DWORD GetRegistryDWORDValue(HKEY hKey, const std::wstring& valueName)
{
    DWORD data;
    DWORD dataSize = sizeof(data);
    DWORD dataType;

    LONG result = RegQueryValueEx(
        hKey,
        valueName.c_str(),
        nullptr,
        &dataType,
        reinterpret_cast<LPBYTE>(&data),
        &dataSize);

    if (result != ERROR_SUCCESS)
    {
        std::wcerr << L"Failed to read registry value: " << valueName << L" (Error: " << result << L")" << std::endl;
        return -1;
    }

    if (dataType != REG_DWORD) {
        std::wcerr << L"Unexpected data type for value: " << valueName << L" (Type: " << dataType << L")" << std::endl;
        return -1;
    }

    return data;
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

    while (RegEnumKeyExW(
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

                    /* capabilities */
                    deviceInfo.isUsbHub = deviceInfo.serviceName == L"USBHUB3";
                    deviceInfo.capabilities = GetDeviceCapabilities(instanceKey);

                    /* preparation for status gathering */
                    deviceInfo.isConnected = false;
                    deviceInfo.isDisabled = false;
                    deviceInfo.isSafeToUnplug = true;

                    /* location */
                    deviceInfo.location = GetRegistryStringValue(instanceKey, L"LocationInformation");

                    /* manufactoring info */
                    deviceInfo.serialNumber = GetDeviceSerialNumber(instanceKey);
                    deviceInfo.usbClass = GetDeviceUsbClass(instanceKey);
                    deviceInfo.usbSubclass = GetDeviceUsbSubclass(instanceKey);
                    deviceInfo.usbProtocol = GetDeviceUsbProtocol(instanceKey);

                    /* IDS */
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

    /* status */
    getDevicesStatus(devices);

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