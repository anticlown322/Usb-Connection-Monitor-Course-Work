#ifndef MY_ALGS_H
#define MY_ALGS_H

struct ThreadArgs {
    std::vector<USBDeviceInfo>* files;
    int left, mid, right;
    DEV_INFO sortType;
};

bool CompareByStringValue(const std::wstring a, const std::wstring b);
bool CompareByBoolValue(const bool a, const bool b);
void Merge(std::vector<USBDeviceInfo>& arr, int left, int mid, int right, DEV_INFO sortType);
DWORD WINAPI MergeThreadProc(LPVOID lpParameter);
void MergeSortMultiThreaded(std::vector<USBDeviceInfo>& arr, int maxThreads, DEV_INFO sortType);

std::wstring GenerateRandomString();

#endif //MY_ALGS_H