#include "EasyBMP.h"
#include <windows.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

const double PI = acos(-1.0);

using namespace std;
using namespace std::chrono;

struct HorizonStripeParams {
    BMP* bmp;
    ofstream* out;
    int stripeStart;
    int stripeEnd;
};

void BlurHorizonStripe(HorizonStripeParams* params, int blurRadius) {
    float stripeWidth = ceil(blurRadius * 2.5);
    int bmpHeight = params->bmp->TellHeight();
    int bmpWidth = params->bmp->TellWidth();
    for (int i = 0; i < bmpHeight; i++) {
        for (int j = params->stripeStart; j < params->stripeEnd; j++) {
            double r = 0, g = 0, b = 0;
            double count = 0;

            for (int iy = i - stripeWidth; iy < i + stripeWidth + 1; iy++) {
                for (int ix = j - stripeWidth; ix < j + stripeWidth + 1; ix++) {
                    auto x = min<int>(bmpWidth - 1, max<int>(0, ix));
                    auto y = min<int>(bmpHeight - 1, max<int>(0, iy));

                    auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
                    auto wght = std::exp(-dsq / (2.0 * blurRadius * blurRadius)) / (PI * 2.0 * blurRadius * blurRadius);

                    RGBApixel pixel = params->bmp->GetPixel(x, y);

                    r += pixel.Red * wght;
                    g += pixel.Green * wght;
                    b += pixel.Blue * wght;
                    count += wght;
                }
            }

            RGBApixel pixel = params->bmp->GetPixel(j, i);
            pixel.Red = std::round(r / count);
            pixel.Green = std::round(g / count);
            pixel.Blue = std::round(b / count);
            params->bmp->SetPixel(j, i, pixel);
            *params->out << clock() << endl;
        }
    }
}

DWORD WINAPI ThreadProc(LPVOID params) {
    auto* blurParams = (HorizonStripeParams*)params;
    BlurHorizonStripe(blurParams, 5);
    ExitThread(0);
}

ofstream* GetOutputFiles(int threadNumber) {
    auto* out = new ofstream[threadNumber];
    for (int i = 0; i < threadNumber; i++) {
        out[i] = ofstream("outputFile" + to_string(i) + ".txt");
    }
    return out;
}

HorizonStripeParams* GetHorizonStripesParams(BMP& bmp, int threadsNumber, ofstream* out) { // thread number для stripes
    HorizonStripeParams* blurParams = new HorizonStripeParams[threadsNumber];
    int width = bmp.TellWidth();
    int stripeWidth = width / threadsNumber;
    int currentWidth = 0;

    for (int i = 0; i < threadsNumber - 1; i++) { // Режем на полосочки
        int nextWidth = currentWidth + stripeWidth;
        HorizonStripeParams stripeParams = { &bmp, &out[i], currentWidth, nextWidth };
        blurParams[i] = stripeParams;
        currentWidth = nextWidth;
    }

    HorizonStripeParams stripeParams = { &bmp, &out[threadsNumber - 1], currentWidth, width };
    blurParams[threadsNumber - 1] = stripeParams;

    return blurParams;
}

void Blur(BMP& bmp, int threadsNumber, int coresNumber, vector<int>& priorities) {
    ofstream* outputFiles = GetOutputFiles(threadsNumber);
    HorizonStripeParams* params = GetHorizonStripesParams(bmp, threadsNumber, outputFiles);

    HANDLE* handles = new HANDLE[threadsNumber];

    for (int i = 0; i < threadsNumber; i++) {
        handles[i] = CreateThread(NULL, 0, &ThreadProc, (HorizonStripeParams*)&params[i], CREATE_SUSPENDED, NULL);
        SetThreadAffinityMask(handles[i], (1 << coresNumber) - 1);
        SetThreadPriority(handles[i], priorities[i]);
    }

    for (int i = 0; i < threadsNumber; i++) {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(threadsNumber, handles, true, INFINITE);

    delete[] outputFiles;
}

int ShowHint(string exception = "") {
    char slash;
    cout << exception << endl;
    cout << "Write '/' for see arguments hint" << endl;
    cin >> slash;
    if (slash == '/') {
        cout << "{pp_lab4.exe} {input file name} {output file name} {threads number} {cores number} {array of priorities: 0, 1 or 2}\n";
    }

    return 1;
}

int main(int argc, char* argv[]) {
    if (argc < 6) {
        return ShowHint();
    }
    char* inputFileName = argv[1];
    char* outputFileName = argv[2];
    int threadsNumber = atoi(argv[3]);
    int coresNumber = atoi(argv[4]);
    vector<int> priorities;

    if (argc < 5 + threadsNumber) {
        return ShowHint();
    }

    try
    {
        for (int i = 0; i < threadsNumber; i++) {
            int priority = atoi(argv[5 + i]);
            if (priority < 0 || priority > 2)
            {
                throw invalid_argument("Priority must be in the range from 0 to 2!");
            }
            priorities.push_back(priority);
        }
    } catch (invalid_argument exception)
    {
        ShowHint(exception.what());
        return 1;
    }
    

    //    vector<int> priorities = {
    //            2, 1, 0
    //    };

    BMP bmp;
    //    bmp.ReadFromFile("inp.bmp");
    //    Blur(bmp, 3, 3, priorities);
    //
    //    bmp.WriteToFile("out.bmp");

    bmp.ReadFromFile(inputFileName);
    Blur(bmp, threadsNumber, coresNumber, priorities);

    bmp.WriteToFile(outputFileName);


    return 0;
}