#include "EasyBMP.h"
#include <windows.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <string>
#include <valarray>

const double PI = acos(-1.0);

using namespace std;
using namespace std::chrono;

struct HorizontalStripeParams { BMP& bmp; int stripeStart; int stripeEnd; };

void BlurHorizontalStripe(HorizontalStripeParams* params, int blurRadius) {
    float stripeWidth = ceil(blurRadius * 2.5);
    int bmpHeight = params->bmp.TellHeight();
    for (int i = 0; i < bmpHeight; i++) {
        for (int j = params->stripeStart; j < params->stripeEnd; j++) {
            double r = 0, g = 0, b = 0;
            double count = 0;

            for (int iy = i - stripeWidth; iy < i + stripeWidth + 1; iy++) {
                for (int ix = j - stripeWidth; ix < j + stripeWidth + 1; ix++) {
                    auto x = min<int>(params->bmp.TellWidth() - 1, max<int>(0, ix));
                    auto y = min<int>(bmpHeight - 1, max<int>(0, iy));

                    auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
                    auto wght = std::exp(-dsq / (2.0 * blurRadius * blurRadius)) / (PI * 2.0 * blurRadius * blurRadius);

                    RGBApixel pixel = params->bmp.GetPixel(x, y);

                    r += pixel.Red * wght;
                    g += pixel.Green * wght;
                    b += pixel.Blue * wght;
                    count += wght;
                }
            }

            RGBApixel pixel = params->bmp.GetPixel(i, j);
            pixel.Red = std::round(r / count);
            pixel.Green = std::round(g / count);
            pixel.Blue = std::round(b / count);
            params->bmp.SetPixel(j, i, pixel);
        }
    }
}

DWORD WINAPI ThreadProc(LPVOID params) {
    auto* blurParams = (HorizontalStripeParams*)params;
    BlurHorizontalStripe(blurParams, 5);
    ExitThread(0);
}

vector<HorizontalStripeParams> GetHorizonStripesParams(BMP& bmp, int stripesNumber) { // thread number для stripes
    vector<HorizontalStripeParams> blurParams;
    int width = bmp.TellWidth();
    int stripeWidth = width / stripesNumber;
    int currentWidth = 0;

    for (int i = 0; i < stripesNumber - 1; i++) { // Режем на полосочки
        int nextWidth = currentWidth + stripeWidth;
        HorizontalStripeParams stripeParams = { bmp, currentWidth, nextWidth };
        blurParams.push_back(stripeParams);
        currentWidth = nextWidth;
    }

    HorizontalStripeParams stripeParams = { bmp, currentWidth, width };
    blurParams.push_back(stripeParams);

    return blurParams;
}

void Blur(vector<HorizontalStripeParams>& blurParams, int threadsNumber, int coresNumber) {
    auto* handles = new HANDLE[threadsNumber];

    for (int i = 0; i < threadsNumber; i++) {
        handles[i] = CreateThread(
            nullptr,
            0,
            &ThreadProc,
            (HorizontalStripeParams*)&blurParams[i],
            CREATE_SUSPENDED,
            nullptr
        );
        SetThreadAffinityMask(handles[i], (1 << coresNumber) - 1);
    }

    for (int i = 0; i < threadsNumber; i++) {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(threadsNumber, handles, true, INFINITE);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cout << "Invalid arguments count\n"
				"Use: pp_lab2.exe <input file name> <output file name> <threads number> <cores number>\n" << endl;
        return 1;
    }

    char* inputFileName = argv[1];
    char* outputFileName = argv[2];
    int threadsNumber = stoi(argv[3]);
    int coresNumber = stoi(argv[4]);

    const steady_clock::time_point startTime = steady_clock::now();

    BMP bmp;
    bmp.ReadFromFile(inputFileName);

    vector<HorizontalStripeParams> blurParams = GetHorizonStripesParams(bmp, threadsNumber);
    Blur(blurParams, threadsNumber, coresNumber);

    bmp.WriteToFile(outputFileName);

    steady_clock::time_point endTime = steady_clock::now();
    const auto runtime = duration_cast<milliseconds>(endTime - startTime).count();
    cout << "runtime: " << runtime << endl;

    return 0;
}