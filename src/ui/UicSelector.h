//
// Created by fab on 19.10.16.
//

#ifndef UIC_UICVIEWER_H
#define UIC_UICVIEWER_H

#include "ItemSelector.h"
#include "../uic/UicDataset.h"
#include "../uic/UicDetector.h"
#include "../line/TextLine.h"

namespace cv {
    namespace uic {
        using namespace std;
        using namespace text;

        class UicSelector : public ItemSelector<UicEntry> {
        private:
//            UicDataset dataset= UicDatasetHelper::splitDataset(UicDatasetHelper::loadAndShuffle("/dataset"),0,0.1);
            UicDataset dataset ;
//            UicDataset testSet, trainSet, validationSet;
            UicEntry::ImageType mode = UicEntry::UIC;
            Mat currentImage;

            void loadCurrentImage();


            void onSelect(int index, UicEntry item);

        public:
            UicSelector();
            UicSelector(UicDataset dataset);

            void setMode(UicEntry::ImageType mode);

            void showCurrentImage();

            inline Mat &getCurrentImage() { return currentImage; }

            inline const UicDataset getDataset() {
                return getItems();
            }

            UicEntry::ImageType getMode();

            Rect getCurrentUicRect();
        };
    }
}

#endif //UIC_UICVIEWER_H
