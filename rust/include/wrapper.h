#pragma once
#include "force-delete-win/src/native.rs.h"
#include "rust/cxx.h"
#include "windows.h"

#include <string>
#include <iostream>
#include <vector>
#include <stdint.h>
#include <bitset>
#include <unordered_map>

#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004
#define NT_SUCCESS(x) ((signed int)(x) >= 0)

#define SystemHandleInformation 16
#define ObjectBasicInformation 0
#define ObjectNameInformation 1
#define ObjectTypeInformation 2


static void throw_runtime_error(HRESULT hr, bool throw_exception = true) {
    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    if (throw_exception) {
        throw std::runtime_error(message.c_str());
    }

    std::cout << message << std::endl;
}


typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;


typedef struct _SYSTEM_HANDLE {
	ULONG ProcessId;
	BYTE ObjectTypeNumber;
	BYTE Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;


typedef struct _SYSTEM_HANDLE_INFORMATION {
	ULONG HandleCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;


typedef enum _POOL_TYPE {
	NonPagedPool,
	PagedPool,
	NonPagedPoolMustSucceed,
	DontUseThisType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned,
	NonPagedPoolCacheAlignedMustS
} POOL_TYPE, *PPOOL_TYPE;


typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING Name;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG TotalPagedPoolUsage;
	ULONG TotalNonPagedPoolUsage;
	ULONG TotalNamePoolUsage;
	ULONG TotalHandleTableUsage;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	ULONG HighWaterPagedPoolUsage;
	ULONG HighWaterNonPagedPoolUsage;
	ULONG HighWaterNamePoolUsage;
	ULONG HighWaterHandleTableUsage;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccess;
	BOOLEAN SecurityRequired;
	BOOLEAN MaintainHandleCount;
	USHORT MaintainTypeList;
	POOL_TYPE PoolType;
	ULONG PagedPoolUsage;
	ULONG NonPagedPoolUsage;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;


class device_map {
private:
    std::unordered_map<std::wstring, std::wstring> m_map;
public:
    device_map() {
        std::bitset<32> const driveExists = GetLogicalDrives();
        for( int i = 0; i < 32; ++i ) {
            if( driveExists[i] ) {
                wchar_t driveLetter = L'A' + i;
                std::wstring rootPath = &driveLetter;
                rootPath.append(L":");

                WCHAR device_name[MAX_PATH];
                QueryDosDeviceW(rootPath.data(), device_name, MAX_PATH);
                auto device_wstr = std::wstring(device_name);
                m_map[device_wstr] = rootPath;
            }
        }
    }

    std::unordered_map<std::wstring, std::wstring> get_map() {
        return m_map;
    }

    operator std::unordered_map<std::wstring, std::wstring>() {
        return m_map;
    }
};


void force_delete_file(rust::Vec<uint16_t> fname);
