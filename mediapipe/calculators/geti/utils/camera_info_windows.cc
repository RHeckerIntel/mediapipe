#include "camera_info.h"
#include <iostream>

#include <windows.h>
#include <dshow.h>

#pragma comment(lib, "strmiids")
#pragma comment(lib, "Ws2_32.lib")

#include <cstring>              // For strcpy
#include <set>

extern "C" {

static void version_to_string(unsigned int version, char* version_str) {
    sprintf(version_str, "%u.%u.%u",
            (version >> 16) & 0xFF,
            (version >> 8) & 0xFF,
            version & 0xFF);
}

int list_cameras(CameraInfo *camera_info, int max_cameras) {
    HRESULT hr;
    ICreateDevEnum *pDevEnum = NULL;
    IEnumMoniker *pEnum = NULL;
    IMoniker *pMoniker = NULL;
    int count = 0;

    // Initialize the COM library
    hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        return 0;
    }

    // Create the System Device Enumerator
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum);
    if (FAILED(hr)) {
        CoUninitialize();
        return 0;
    }

    // Create an enumerator for the video capture category
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr == S_OK) {
        // Enumerate the monikers
        while (pEnum->Next(1, &pMoniker, NULL) == S_OK && count < max_cameras) {
            IPropertyBag *pPropBag;
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropBag);
            if (SUCCEEDED(hr)) {
                VARIANT var;
                VariantInit(&var);

                // Get the friendly name of the device
                hr = pPropBag->Read(L"FriendlyName", &var, 0);
                if (SUCCEEDED(hr)) {
                    WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, camera_info[count].device, sizeof(camera_info[count].device), NULL, NULL);
                }
                VariantClear(&var);

                // Get other device properties if needed
                // For example, you can read "DevicePath", "DriverDesc", etc. from the property bag

                pPropBag->Release();
            }
            pMoniker->Release();
            count++;
        }
        pEnum->Release();
    }
    pDevEnum->Release();
    CoUninitialize();
    return count;
}


} // extern "C"
