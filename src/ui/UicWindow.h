//
// Created by fab on 11.01.17.
//

#ifndef UIC_UICDEBUGWND_H
#define UIC_UICDEBUGWND_H

#include "../uic/UicDetector.h"
#include "../uic/UicDebugHelper.h"
#include "../uic/UicEvaluator.h"
#include "UicDetectorSelector.h"

using namespace std;
using namespace cv::uic;

class UicWindow {
private:
    UicSelector uics;
    UicDetectorSelector detectors;
    bool useWhitelist = false;

    void testCurrentEntry();

public:
    UicWindow(UicDataset dataset, vector<PUicProcess> detectors);
    UicWindow();
    void open();

    void calcCurrentStats();
    void calcStats();
};


#endif //UIC_UICDEBUGWND_H
