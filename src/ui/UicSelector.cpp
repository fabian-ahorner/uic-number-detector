//
// Created by fab on 19.10.16.
//

#include "UicSelector.h"
#include "../uic/UicDebugHelper.h"
#include <opencv2/highgui.hpp>


namespace cv {
    namespace uic {
        UicSelector::UicSelector() : UicSelector(UicDatasetHelper::splitDataset(
                UicDatasetHelper::loadAndShuffle("/home/fab/datasets/uic/full"), 0, 1)) {
        }

        UicSelector::UicSelector(UicDataset dataset)
                : ItemSelector(dataset) {
            this->dataset = dataset;
            namedWindow("original", WINDOW_NORMAL);
            setIndex(0);
        }

        void UicSelector::onSelect(int index, UicEntry item) {
            showCurrentImage();
        }

        void UicSelector::loadCurrentImage() {
            currentImage = getCurrent().getImage(mode);
        }

        void UicSelector::showCurrentImage() {
            loadCurrentImage();
//            imshow("original", currentImage);
            UicDebugHelper::show("original", UicDebugHelper::drawDigits(currentImage, getCurrent(), mode));
            cout << getCurrentIndex() << ": " << getCurrent().name << endl;
        }

        void UicSelector::setMode(UicEntry::ImageType mode) {
            this->mode = mode;
            showCurrentImage();
        }

        UicEntry::ImageType UicSelector::getMode() {
            return mode;
        }

        Rect UicSelector::getCurrentUicRect() {
            return getCurrent().getUicBox(mode);
        }
    }
}