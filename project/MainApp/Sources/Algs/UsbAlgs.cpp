#include "MainApp/Headers/App/general.h"
#include "MainApp/Headers/Algs/UsbAlgs.h"

std::vector<USBDeviceInfo> getUSBDevicesInformation()
{
    std::vector<USBDeviceInfo> devices;

    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        NULL,
        NULL,
        NULL,
        DIGCF_ALLCLASSES | DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to get device info set." << std::endl;
        return devices;
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
    {
        if (IsUSBDevice(hDevInfo, &devInfoData))
        {
            USBDeviceInfo device;
            DWORD buffersize = 0;
            BYTE buffer[1024];

            // getting device description
            while (!SetupDiGetDeviceRegistryProperty(
                hDevInfo,
                &devInfoData,
                SPDRP_DEVICEDESC,
                NULL,
                buffer,
                sizeof(buffer),
                &buffersize))
            {
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                    // buffer size is enough. just repeat the call
                }
                else {
                    break; //if another error then stop
                }
            }

            // getting device class GUID
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_CLASSGUID, NULL, buffer, sizeof(buffer), NULL);
            CLSIDFromString((LPCOLESTR)(wchar_t*)buffer, &device.deviceClassGUID);
            device.deviceDescription = (wchar_t*)buffer;

            // getting ID
            TCHAR devID[MAX_DEVICE_ID_LEN];
            if (CM_Get_Device_ID(devInfoData.DevInst, devID, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS) {
                device.deviceID = devID;
            }

            // getting Hardware ID (for more detailed info)
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_HARDWAREID, NULL, buffer, sizeof(buffer), NULL);
            device.hardwareID = (wchar_t*)buffer;

            // getting device class
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_CLASS, NULL, buffer, sizeof(buffer), NULL);
            device.deviceClass = (wchar_t*)buffer;

            // getting device status
            ULONG status = 0;
            ULONG problemNumber = 0;
            if (CM_Get_DevNode_Status(&status, &problemNumber, devInfoData.DevInst, 0) == CR_SUCCESS) {
                device.isConnected = (status & DN_DRIVER_LOADED) && (status & DN_STARTED) && !(status & DN_HAS_PROBLEM);
                device.isDisabled = (status & DN_HAS_PROBLEM) && (problemNumber == CM_PROB_DISABLED);
            }
            else {
                device.isConnected = false;
                device.isDisabled = false;
            }

            devices.push_back(device);
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    return devices;
}

bool IsUSBDevice(HDEVINFO hDevInfo, PSP_DEVINFO_DATA pDeviceInfoData) 
{
    SP_DEVICE_INTERFACE_DATA interfaceData;
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // Перебор интерфейсов
    for (
        DWORD dwIndex = 0; 
        SetupDiEnumDeviceInterfaces(
            hDevInfo, 
            pDeviceInfoData, 
            &GUID_DEVINTERFACE_USB_DEVICE, 
            dwIndex, 
            &interfaceData); 
        dwIndex++) 
    {
        DWORD requiredSize = 0;
        PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetail = NULL;

        SetupDiGetDeviceInterfaceDetail(
            hDevInfo, 
            &interfaceData, 
            NULL, 
            0, 
            &requiredSize, 
            NULL);

        pInterfaceDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LMEM_FIXED, requiredSize);
        if (pInterfaceDetail == NULL) 
        {
            std::cerr << "Failed to allocate memory." << std::endl;
            return false;
        }

        pInterfaceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // Получаем детали интерфейса
        if (SetupDiGetDeviceInterfaceDetail(
            hDevInfo, 
            &interfaceData, 
            pInterfaceDetail, 
            requiredSize, 
            NULL, 
            NULL)) 
        {
            LocalFree(pInterfaceDetail);
            return true;
        }

        LocalFree(pInterfaceDetail);
    }

    return false; 
}