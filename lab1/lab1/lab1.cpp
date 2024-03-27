#include <windows.h>
#include <string>
#include <iostream>

DWORD WINAPI PrintThreadWorkTime(const int number) {
    try {
        std::cout << "Thread number " << number << " make job" << std::endl;
        ExitThread(0);
    }
    catch (int e) {
        std::cout << "Thread error, exception code:" << e << std::endl;
    }
}

void StartThreads(int threadCount) {
    auto* handles = new HANDLE[threadCount];
    for (int i = 0; i < threadCount; i++) {
        handles[i] = CreateThread(
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(&PrintThreadWorkTime),
            reinterpret_cast<LPVOID>(i),
            CREATE_SUSPENDED,
            nullptr
        );

        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(2, handles, true, INFINITE);
}

int main([[maybe_unused]] int argc, char* argv[]) {

    int threadCount = std::stoi(argv[1]);
    StartThreads(threadCount);

    return 0;
}


