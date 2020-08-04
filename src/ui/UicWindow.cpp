//
// Created by fab on 11.01.17.
//

#include "UicWindow.h"
#include "opencv2/highgui.hpp"

void UicWindow::open() {
    uics.showCurrentImage();
    do {
        int key = (char) waitKey(0);
        switch ((char) key) {
            case 'd':
                uics.next();
                break;
            case 'a':
                uics.previous();
                break;
            case 'o':
                uics.setMode(UicEntry::ORIGINAL);
                break;
            case 'u':
                uics.setMode(UicEntry::UIC);
                break;
            case 't':
                uics.setMode(UicEntry::TRAIN);
                break;
            case 'i':
                uics.setMode(UicEntry::DIGITS);
                break;
            case 'e':
                if (useWhitelist)
                    UicEvaluator::evaluateUicStrings(detectors.getCurrentDetector(), uics.getDataset(), uics.getMode(),
                                                     getWhitelist(uics.getDataset()));
                else
                    UicEvaluator::evaluateUicStrings(detectors.getCurrentDetector(), uics.getDataset(), uics.getMode());
                break;
//            case 'c':
//                UicEvaluator::evaluateCharDetector(
//                        detectors.getCurrentDetector()->getChildPipeline()->getChildPipeline()->getChildPipeline(), uics.getDataset(),
//                        uics.getMode());
//                break;
            case 'w':
                detectors.previous();
                break;
            case 'l':
                useWhitelist = !useWhitelist;
                cout << "Use whitelist:" << useWhitelist << endl;
                break;
            case 's':
                detectors.next();
                break;
            case ' ':
                testCurrentEntry();
                break;
            case 'c':
                calcCurrentStats();
                break;
            case 'v':
                calcStats();
                break;
            default:
                printf("Key: %d 0x%x '%c'\r\n", key, key, key);
                return;
        }
    } while (1);
}

void UicWindow::testCurrentEntry() {
    TimeLogger::clear();
    const Mat &img = uics.getCurrentImage();
    PUicResultVector uicResults = UicDebugHelper::computeAndShowIntermediates(img, detectors.getCurrentDetector());
    UicEvaluator::evaluateResult(uics.getCurrent(), uicResults, uics.getMode());
}

void UicWindow::calcCurrentStats() {
    int height = uics.getCurrent().getMinDigitHeight();
    Scalar rms = uics.getCurrent().getDigitRMS();
    double norm = cv::norm(rms);
    cout << "Height=" << height << " |sd|=" << norm << endl;
}

void UicWindow::calcStats() {
    for (auto &uic:uics.getItems()) {
        int height = uic.getMinDigitHeight();
        Scalar rms = uic.getDigitRMS();
        double norm = cv::norm(rms);
        cout << height << "\t" << norm << "\t" << uic.quality << endl;
    }
}

UicWindow::UicWindow(UicDataset dataset, vector<PUicProcess> detectors) : uics(UicSelector(dataset)),
                                                                          detectors(UicDetectorSelector(detectors)) {
}

UicWindow::UicWindow() : uics(UicSelector()), detectors(UicDetectorSelector()) {
}
