#include <iostream>
#include <cmath>
#include "chrono"

using namespace std;
using namespace std::chrono;

double GetPiViaReduction(int count) {
    steady_clock::time_point startTime = steady_clock::now();
    double result = 0;
#pragma omp parallel for reduction(+ : result)
    for (int n = 0; n < count; n++) result += pow(-1, n) / ((2 * n) + 1);

    auto resultTime = duration_cast<microseconds>(steady_clock::now() - startTime).count();
    cout << "Reduction: " << resultTime << endl;
    return 4 * result;
}

double GetCorrectPiViaAsyncFor(int count) {
    steady_clock::time_point startTime = steady_clock::now();
    double result = 0;
#pragma omp parallel for
    for (int n = 0; n < count; n++) {
#pragma omp atomic update
        result += pow(-1, n) / ((2 * n) + 1);
    }

    auto resultTime = duration_cast<microseconds>(steady_clock::now() - startTime).count();
    cout << "AsyncFor: " << resultTime << endl;
    return 4 * result;
}

double GetIncorrectPiViaAsyncFor(int count) {
    steady_clock::time_point startTime = steady_clock::now();
    double result = 0;
#pragma omp parallel for
    for (int n = 0; n < count; n++) result += pow(-1, n) / ((2 * n) + 1);
    auto resultTime = duration_cast<microseconds>(steady_clock::now() - startTime).count();
    cout << "IncAsyncFor: " << resultTime << endl;
    return 4 * result;
}

double GetPiViaSyncFor(int count) {
    steady_clock::time_point startTime = steady_clock::now();
    double result = 0;
    for (int n = 0; n < count; n++) {
        result += pow(-1, n) / ((2 * n) + 1);
    }
    auto resultTime = duration_cast<microseconds>(steady_clock::now() - startTime).count();
    cout << "SyncFor: " << resultTime << endl;
    return 4 * result;
}

int main() {
    cout << GetPiViaSyncFor(8000000) << endl;
    cout << GetIncorrectPiViaAsyncFor(8000000) << endl;
    // На малом количестве выигрывет atomic, а на больших объемах reduction
    cout << GetCorrectPiViaAsyncFor(8000000) << endl;
    cout << GetPiViaReduction(8000000) << endl;

    return 0;
}
