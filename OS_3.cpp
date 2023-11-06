#include <iostream>
#include <Windows.h>
#include "HeapClass.h"


using namespace std;

void PrintMemoryInfo(MEMORY_BASIC_INFORMATION memInfo) {
    cout << "Base Address: 0x" << hex << memInfo.BaseAddress << dec << endl;
    cout << "Region Size: " << memInfo.RegionSize / (1024 * 1024) << " MB" << endl;


    const DWORD state = memInfo.State;
    const char* stateStr = nullptr;
    switch (state) {
    case MEM_COMMIT: stateStr = "MEM_COMMIT"; break;
    case MEM_RESERVE: stateStr = "MEM_RESERVE"; break;
    case MEM_FREE: stateStr = "MEM_FREE"; break;
    default: stateStr = "UNKNOWN";
    }
    cout << "State: " << stateStr << endl;

    const DWORD protect = memInfo.Protect;
    const char* protectStr = nullptr;
    switch (protect) {
    case PAGE_NOACCESS: protectStr = "PAGE_NOACCESS"; break;
    case PAGE_READONLY: protectStr = "PAGE_READONLY"; break;
    case PAGE_READWRITE: protectStr = "PAGE_READWRITE"; break;
    case PAGE_WRITECOPY: protectStr = "PAGE_WRITECOPY"; break;
    case PAGE_EXECUTE: protectStr = "PAGE_EXECUTE"; break;
    case PAGE_EXECUTE_READ: protectStr = "PAGE_EXECUTE_READ"; break;
    case PAGE_EXECUTE_READWRITE: protectStr = "PAGE_EXECUTE_READWRITE"; break;
    case PAGE_EXECUTE_WRITECOPY: protectStr = "PAGE_EXECUTE_WRITECOPY"; break;
    case PAGE_GUARD: protectStr = "PAGE_GUARD"; break;
    case PAGE_NOCACHE: protectStr = "PAGE_NOCACHE"; break;
    case PAGE_WRITECOMBINE: protectStr = "PAGE_WRITECOMBINE"; break;
    default: protectStr = "UNKNOWN";
    }
    cout << "Protect: " << protectStr << endl;

    const DWORD type = memInfo.Type;
    const char* typeStr = nullptr;
    switch (type) {
    case MEM_PRIVATE: typeStr = "MEM_PRIVATE"; break;
    case MEM_MAPPED: typeStr = "MEM_MAPPED"; break;
        // Додайте інші варіанти за потреби
    default: typeStr = "UNKNOWN";
    }
    //cout << "Type: 0x" << hex << memInfo.Type << dec << endl;
    cout << "Type: " << typeStr << endl;

    cout << "--------------------------------------------------" << endl;
}


class CExtBuffer {
public:
    STDMETHODIMP FInit(
        ULONG cItemMax,     //@parm IN | Maximum number of items ever
        ULONG cbItem,       //@parm IN | Size of each item, in bytes
        ULONG cbPage        //@parm IN | Size of system page size (from SysInfo)
    ) {
        BYTE* pb;

        m_cbReserved = ((cbItem * cItemMax) / cbPage + 1) * cbPage;
        m_rgItem = (BYTE*)VirtualAlloc(NULL, m_cbReserved, MEM_RESERVE, PAGE_READWRITE);

        if (m_rgItem == NULL)
            return ResultFromScode(E_OUTOFMEMORY);

        m_cbItem = cbItem;
        m_dbAlloc = (cbItem / cbPage + 1) * cbPage;
        pb = (BYTE*)VirtualAlloc(m_rgItem, m_dbAlloc, MEM_COMMIT, PAGE_READWRITE);
        if (pb == NULL) {
            VirtualFree((VOID*)m_rgItem, 0, MEM_RELEASE);
            m_rgItem = NULL;
            return ResultFromScode(E_OUTOFMEMORY);
        }

        m_cbAlloc = m_dbAlloc;

        return ResultFromScode(S_OK);
    }

private:
    ULONG m_cbReserved;
    ULONG m_cbItem;
    ULONG m_dbAlloc;
    ULONG m_cbAlloc;
    BYTE* m_rgItem;
};

int main() {
    // Task 1: Print memory regions of the current process
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    LPVOID address = sysInfo.lpMinimumApplicationAddress;

    while (address < sysInfo.lpMaximumApplicationAddress) {
        MEMORY_BASIC_INFORMATION memInfo;
        if (VirtualQuery(address, &memInfo, sizeof(memInfo))) {
            PrintMemoryInfo(memInfo);
            address = (LPBYTE)memInfo.BaseAddress + memInfo.RegionSize;
        }
        else {
            break;
        }
    }

    // Task 2: Create and use additional heaps
    CustomHeap heap1;
    CustomHeap heap2;

    int* data1 = (int*)heap1.Allocate(sizeof(int) * 5);
    for (int i = 0; i < 5; i++) {
        data1[i] = i;
        cout << "Heap 1, Element " << i << ": " << data1[i] << endl;
    }

    int* data2 = (int*)heap2.Allocate(sizeof(int) * 3);
    for (int i = 0; i < 3; i++) {
        data2[i] = i * 2;
        cout << "Heap 2, Element " << i << ": " << data2[i] << endl;
    }

    heap1.Free(data1);
    heap2.Free(data2);

    // Task 3: Call FInit to allocate and print memory information
    CExtBuffer extBuffer;
    extBuffer.FInit(1, 1024, 4096);

    return 0;
}