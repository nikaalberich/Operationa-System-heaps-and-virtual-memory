#pragma once
#include <iostream>
#include <Windows.h>

//код включає в себе роботу з кастомним алокатором пам'яті, 
//який використовує власну купу для виділення та звільнення пам'яті замість стандартного оператора new та delete.

using namespace std;

class CustomHeap {
public:
    CustomHeap() {
        heapHandle = HeapCreate(0, 0, 0);
    }

    ~CustomHeap() {
        if (heapHandle != nullptr) {
            HeapDestroy(heapHandle);
        }
    }

    void* Allocate(size_t size) {
        return HeapAlloc(heapHandle, 0, size);
    }

    void Free(void* ptr) {
        if (ptr != nullptr) {
            HeapFree(heapHandle, 0, ptr);
        }
    }

private:
    HANDLE heapHandle;
};

// Перевантаження операторів new та delete
void* operator new(size_t size) {
    CustomHeap customHeap;
    return customHeap.Allocate(size);
}

void operator delete(void* ptr) {
    CustomHeap customHeap;
    customHeap.Free(ptr);
}

class MyClass {
public:
    MyClass(int val) : value(val) {}

    void PrintValue() {
        cout << "Value: " << value << endl;
    }

private:
    int value;
};
