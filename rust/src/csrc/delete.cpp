#include "wrapper.h"
#include "Windows.h"
#include "Shobjidl_core.h"
#include "objbase.h"

#include <cstdlib>
#include <string>
#include <stdio.h>
#include <iostream>
#include <bitset>
#include <map>


void force_delete_file(rust::Vec<uint16_t> fname) {

    PCWSTR fname_pcwstr = PCWSTR(fname.data());
    auto fname_wstr = std::wstring(fname_pcwstr);
    device_map* drives = new device_map();
    auto drives_map = drives->get_map();

    typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
        ULONG                    SystemInformationClass,
        PVOID                    SystemInformation,
        ULONG                    SystemInformationLength,
        PULONG                   ReturnLength);

    typedef NTSTATUS(NTAPI *_NtDuplicateObject)(
        HANDLE SourceProcessHandle,
        HANDLE SourceHandle,
        HANDLE TargetProcessHandle,
        PHANDLE TargetHandle,
        ACCESS_MASK DesiredAccess,
        ULONG Attributes,
        ULONG Options
	);

    typedef NTSTATUS(NTAPI* _NtQueryObject)(
        HANDLE               ObjectHandle,
        ULONG                ObjectInformationClass,
        PVOID                ObjectInformation,
        ULONG                ObjectInformationLength,
        PULONG               ReturnLength
    );


    _NtQuerySystemInformation NtQuerySystemInformation = nullptr;
    _NtDuplicateObject NtDuplicateObject = nullptr;
    _NtQueryObject NtQueryObject = nullptr;

    HMODULE hDLL = LoadLibrary(TEXT("ntdll.dll"));
    NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(hDLL, "NtQuerySystemInformation");
    NtDuplicateObject = (_NtDuplicateObject)GetProcAddress(hDLL, "NtDuplicateObject");
    NtQueryObject = (_NtQueryObject)GetProcAddress(hDLL, "NtQueryObject");

    NTSTATUS status;
    PSYSTEM_HANDLE_INFORMATION handleInfo;
    ULONG handleInfoSize = 0x10000;
    ULONG pid;
    HANDLE processHandle;
    ULONG i;

    handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(handleInfoSize);

    /* NtQuerySystemInformation won't give us the correct buffer size,
    so we guess by doubling the buffer size. */
    while ((status = NtQuerySystemInformation(
        SystemHandleInformation,
        handleInfo,
        handleInfoSize,
        NULL
        )) == STATUS_INFO_LENGTH_MISMATCH)
        handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, handleInfoSize *= 2);

    /* NtQuerySystemInformation stopped giving us STATUS_INFO_LENGTH_MISMATCH. */
    if (!NT_SUCCESS(status)) {
        printf("NtQuerySystemInformation failed!\n");
        return;
    }

    for (i = 0; i < handleInfo->HandleCount; i++) {
        SYSTEM_HANDLE handle = handleInfo->Handles[i];
        HANDLE dupHandle = NULL;
        POBJECT_TYPE_INFORMATION objectTypeInfo;
        PVOID objectNameInfo;
        UNICODE_STRING objectName;
        ULONG returnLength;

        /* Open a handle to the process associated with the handle */
        if (!(processHandle = OpenProcess(
                PROCESS_DUP_HANDLE, FALSE, handle.ProcessId)))
            continue;

        /* Check if this handle belongs to the PID the user specified. */
        // if (handle.ProcessId != pid)
        //     continue;

        /* Duplicate the handle so we can query it. */
        if (!NT_SUCCESS(NtDuplicateObject(
                processHandle,
                (HANDLE) handle.Handle,
                GetCurrentProcess(),
                &dupHandle,
                0,
                0,
                0))) {
            continue;
        }

        /* Query the object type. */
        objectTypeInfo = (POBJECT_TYPE_INFORMATION)malloc(0x1000);
        if (!NT_SUCCESS(NtQueryObject(
                dupHandle,
                ObjectTypeInformation,
                objectTypeInfo,
                0x1000,
                NULL))) {
            // printf("[%#x] Error!\n", handle.Handle);
            CloseHandle(dupHandle);
            continue;
        }


        /* Query the object name (unless it has an access of
        0x0012019f, on which NtQueryObject could hang. */

        if ((handle.GrantedAccess & 0x120089) == 0x120089 || handle.GrantedAccess == 0x0012019f || handle.GrantedAccess == 0x001a019f || handle.GrantedAccess == 0x00120189 || handle.GrantedAccess == 0x0016019f || handle.GrantedAccess == 0x00120089) {
            /* We have the type, so display that. */
            // printf(
            //     "[%#x] %.*S: (did not get name)\n",
            //     handle.Handle,
            //     objectTypeInfo->Name.Length / 2,
            //     objectTypeInfo->Name.Buffer
            //     );
            free(objectTypeInfo);
            CloseHandle(dupHandle);
            continue;
        }

        objectNameInfo = malloc(0x1000);
        if (!NT_SUCCESS(NtQueryObject(
                dupHandle,
                ObjectNameInformation,
                objectNameInfo,
                0x1000,
                &returnLength
            ))) {
            /* Reallocate the buffer and try again. */
            objectNameInfo = realloc(objectNameInfo, returnLength);
            if (!NT_SUCCESS(NtQueryObject(
                    dupHandle,
                    ObjectNameInformation,
                    objectNameInfo,
                    returnLength,
                    NULL))) {
                /* We have the type name, so just display that. */
                // printf(
                //     "[%#x] %.*S: (could not get name)\n",
                //     handle.Handle,
                //     objectTypeInfo->Name.Length / 2,
                //     objectTypeInfo->Name.Buffer
                //     );
                free(objectTypeInfo);
                free(objectNameInfo);
                CloseHandle(dupHandle);
                continue;
            }
        }

        /* Cast our buffer into an UNICODE_STRING. */
        objectName = *(PUNICODE_STRING)objectNameInfo;

        /* Print the information! */
        if (objectName.Length) {
            /* The object has a name. */
            auto obj_type = std::wstring(objectTypeInfo->Name.Buffer, 0, objectTypeInfo->Name.Length / 2);
            if(obj_type == std::wstring(L"File")) {
                // printf(
                //     "---- [%#x] %.*S: %.*S\n",
                //     handle.Handle,
                //     objectTypeInfo->Name.Length / 2,
                //     objectTypeInfo->Name.Buffer,
                //     objectName.Length / 2,
                //     objectName.Buffer);

                std::wstring file_path(objectName.Buffer, 0, objectName.Length / 2);
                // file_path.find(L"\\Device")
                for(auto kv: drives_map) {
                    std::wstring device_prefix, drive_letter;
                    std::tie(device_prefix, drive_letter) = kv;
                    auto pos = file_path.find(device_prefix);
                    if(pos != std::wstring::npos) {
                        file_path = file_path.replace(pos, pos + device_prefix.length(), drive_letter);
                        break;
                    }
                }

                if(fname_wstr == file_path) {
                    // std::wcout << file_path << " " << (fname_wstr == file_path) << " " << handle.ProcessId << std::endl;
                    CloseHandle(dupHandle);

                    if (NT_SUCCESS(NtDuplicateObject(
                            processHandle,
                            (HANDLE) handle.Handle,
                            GetCurrentProcess(),
                            &dupHandle,
                            0,
                            0,
                            DUPLICATE_CLOSE_SOURCE))) {
                        // This will try to close the requested handle by force
                        CloseHandle(dupHandle);
                        free(objectTypeInfo);
                        free(objectNameInfo);

                        auto attrs = GetFileAttributesW(file_path.data());
                        if(attrs & FILE_ATTRIBUTE_DIRECTORY) {
                            RemoveDirectoryW(file_path.data());
                        } else {
                            DeleteFileW(file_path.data());
                        }
                        continue;
                    }
                }
            }
        }

        free(objectTypeInfo);
        free(objectNameInfo);
        CloseHandle(dupHandle);
    }

    free(handleInfo);
}
