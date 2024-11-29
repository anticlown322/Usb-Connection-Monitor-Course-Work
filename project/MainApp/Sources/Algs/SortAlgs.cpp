#include "MainApp/Headers/App/General.h"
#include "MainApp/Headers/Algs/SortAlgs.h"

bool CompareByStringValue(const std::wstring a, const std::wstring b) 
{
    return a < b;
}

bool CompareByBoolValue(const bool a, const bool b) 
{
    return !a && b;
}

void Merge(std::vector<USBDeviceInfo>& files, int left, int mid, int right, DEV_INFO sortType)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<USBDeviceInfo> leftArr(n1);
    std::vector<USBDeviceInfo> rightArr(n2);

    for (int i = 0; i < n1; i++)
        leftArr[i] = files[left + i];
    for (int j = 0; j < n2; j++)
        rightArr[j] = files[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        bool compareResult = false;

        switch (sortType)
        {
            case DEV_INFO::DESC:    compareResult = CompareByStringValue(leftArr[i].description, rightArr[j].description); break;
            case DEV_INFO::MFG:     compareResult = CompareByStringValue(leftArr[i].manufactoring, rightArr[j].manufactoring); break;
            case DEV_INFO::SERVICE: compareResult = CompareByStringValue(leftArr[i].serviceName, rightArr[j].serviceName); break;

            case DEV_INFO::IS_CONNECTED:      compareResult = CompareByBoolValue(leftArr[i].isConnected, rightArr[j].isConnected); break;
            case DEV_INFO::IS_DIASBLED:       compareResult = CompareByBoolValue(leftArr[i].isDisabled, rightArr[j].isDisabled); break;
            case DEV_INFO::IS_SAFE_TO_UNPLUG: compareResult = CompareByBoolValue(leftArr[i].isSafeToUnplug, rightArr[j].isSafeToUnplug); break;
            case DEV_INFO::IS_USB_HUB:        compareResult = CompareByBoolValue(leftArr[i].isUsbHub, rightArr[j].isUsbHub); break;

            case DEV_INFO::CAPABILITIES: compareResult = CompareByStringValue(leftArr[i].capabilities, rightArr[j].capabilities); break;

            case DEV_INFO::LOCATION: compareResult = CompareByStringValue(leftArr[i].location, rightArr[j].location); break;

            case DEV_INFO::SERIAL_NUM:   compareResult = CompareByStringValue(leftArr[i].serialNumber, rightArr[j].serialNumber); break;
            case DEV_INFO::USB_CLASS:    compareResult = CompareByStringValue(leftArr[i].usbClass, rightArr[j].usbClass); break;
            case DEV_INFO::USB_SUBCLASS: compareResult = CompareByStringValue(leftArr[i].usbSubclass, rightArr[j].usbSubclass); break;
            case DEV_INFO::USB_PROTOCOL: compareResult = CompareByStringValue(leftArr[i].usbProtocol, rightArr[j].usbProtocol); break;

            case DEV_INFO::ID:            compareResult = CompareByStringValue(leftArr[i].id, rightArr[j].id); break;
            case DEV_INFO::DEV_GUID:      compareResult = CompareByStringValue(leftArr[i].devClassGUID, rightArr[j].devClassGUID); break;
            case DEV_INFO::HARDWARE_ID:   compareResult = CompareByStringValue(leftArr[i].hardwareID, rightArr[j].hardwareID); break;
            case DEV_INFO::PARENT_PREFIX: compareResult = CompareByStringValue(leftArr[i].parentIDPrefix, rightArr[j].parentIDPrefix); break;
        }

        if (compareResult) {
            files[k] = leftArr[i];
            i++;
        }
        else {
            files[k] = rightArr[j];
            j++;
        }

        k++;
    }

    while (i < n1) {
        files[k] = leftArr[i];
        i++;
        k++;
    }

    while (j < n2) {
        files[k] = rightArr[j];
        j++;
        k++;
    }
}

DWORD WINAPI MergeThreadProc(LPVOID lpParameter)
{
    ThreadArgs* args = (ThreadArgs*)lpParameter;
    Merge(*args->files, args->left, args->mid, args->right, args->sortType);
    delete args;
    return 0;
}

void MergeSortMultiThreaded(std::vector<USBDeviceInfo>& files, int maxThreads, DEV_INFO sortType)
{
    int n = files.size();
    std::vector<HANDLE> hThreads;
    hThreads.reserve(maxThreads);

    for (int subSize = 1; subSize < n; subSize *= 2)
    {
        for (int left = 0; left < n; left += 2 * subSize)
        {
            int mid = (left + subSize - 1) < (n - 1) ? (left + subSize - 1) : (n - 1);
            int right = (left + 2 * subSize - 1) < (n - 1) ? (left + 2 * subSize - 1) : (n - 1);

            if (left < right)
            {
                if (hThreads.size() < maxThreads)
                {
                    ThreadArgs* args = new ThreadArgs{ &files, left, mid, right, sortType };
                    HANDLE hThread = CreateThread(NULL, 0, MergeThreadProc, args, 0, NULL);

                    if (hThread == NULL)
                    {
                        delete args;
                        continue;
                    }

                    hThreads.push_back(hThread);
                }
                else
                {
                    DWORD endedThreadIndex = WaitForMultipleObjects(1, &hThreads[0], FALSE, INFINITE);
                    CloseHandle(hThreads[endedThreadIndex - WAIT_OBJECT_0]);
                    hThreads.erase(hThreads.begin() + endedThreadIndex - WAIT_OBJECT_0);

                    ThreadArgs* args = new ThreadArgs{ &files, left, mid, right, sortType };
                    HANDLE hThread = CreateThread(NULL, 0, MergeThreadProc, args, 0, NULL);
                    hThreads.push_back(hThread);
                }
            }
        }

        if (!hThreads.empty())
        {
            WaitForMultipleObjects(hThreads.size(), &hThreads[0], TRUE, INFINITE);

            for (HANDLE hThread : hThreads)
            {
                CloseHandle(hThread);
            }

            hThreads.clear();
        }
    }
}