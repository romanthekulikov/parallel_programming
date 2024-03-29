#include <windows.h>
#include <string>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>

using namespace std;
using namespace std::chrono;

CRITICAL_SECTION criticalSection;
std::ofstream outputFile1, outputFile2;
steady_clock::time_point startTime;

DWORD WINAPI PrintThreadWorkTime(const int number) {
    EnterCriticalSection(&criticalSection);

    steady_clock::time_point startTimeThread = startTime;

    LeaveCriticalSection(&criticalSection);

    for (int i = 0; i < 20; i++) {
        try {
            steady_clock::time_point endTime = steady_clock::now();
            auto runtime = duration_cast<microseconds>(endTime - startTimeThread).count();
            string stringRuntime = to_string((double)runtime / 1000);
            replace(stringRuntime.begin(), stringRuntime.end(), '.', ',');
            if (number == 1) {
                outputFile1 << number << " | " << stringRuntime << endl;
            }
            else {
                outputFile2 << number << " | " << stringRuntime << endl;
            }


        }
        catch (...) {
            cout << "Thread process error, exception code:" << endl;
        }
    }

    ExitThread(0);
}

void WriteThreadTimeWorkToFiles() {
    auto* handles = new HANDLE[2];
    handles[0] = CreateThread(
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(&PrintThreadWorkTime),
        reinterpret_cast<LPVOID>(1),
        CREATE_SUSPENDED,
        nullptr
    );

    handles[1] = CreateThread(
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(&PrintThreadWorkTime),
        reinterpret_cast<LPVOID>(2),
        CREATE_SUSPENDED,
        nullptr
    );

    //    SetThreadPriority(handles[0], THREAD_PRIORITY_ABOVE_NORMAL);
    //    SetThreadPriority(handles[1], THREAD_PRIORITY_BELOW_NORMAL);

    ResumeThread(handles[0]);
    ResumeThread(handles[1]);

    WaitForMultipleObjects(2, handles, true, INFINITE);
}

int main() {
    cout << "Press Enter to Continue...";
    cin.ignore();
    startTime = steady_clock::now();
    outputFile1.open("outputFile1.txt");
    outputFile2.open("outputFile2.txt");

    InitializeCriticalSection(&criticalSection);
    if (outputFile1.is_open() && outputFile2.is_open()) {
        WriteThreadTimeWorkToFiles();
    }
    DeleteCriticalSection(&criticalSection);

    return 0;
}