//
// Created by fab on 11.01.17.
//

#ifndef UIC_UICDETECTORSELECTOR_H
#define UIC_UICDETECTORSELECTOR_H

#include "ItemSelector.h"
#include <string>
#include "../uic/UicDetector.h"

using namespace cv::uic;
using namespace cv;

class UicDetectorSelector : public ItemSelector<PUicProcess> {
private:
    vector<PUicProcess> detectors = createDetectors();


    void onSelect(int index, PUicProcess detector);

public:
    static vector<PUicProcess> createDetectors();
    UicDetectorSelector();
    UicDetectorSelector(vector<PUicProcess> detectors);

    PUicProcess getCurrentDetector();
};


#endif //UIC_UICDETECTORSELECTOR_H
