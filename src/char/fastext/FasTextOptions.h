//
// Created by fab on 05.03.17.
//

#ifndef UIC_FASTEXTOPTIONS_H
#define UIC_FASTEXTOPTIONS_H

#include "../CharacterDetector.h"

namespace cv {
    namespace text {
        struct FasTextOptions : public CharDetectorOptions {
            string classifierNM1Path = "/home/fab/dev/cpp/uic/models/trained_classifierNM1.xml";
            string classifierNM2Path = "/home/fab/dev/cpp/uic/models/trained_classifierNM2.xml";
            int minThreshold = 16;
            float minProbabilityNM1 = 0;
            float minProbabilityNM2 = -.5f;
            bool nonMaxSuppression = false;
            float scaleFactor = 1.6;
            int channels = ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_GRADIENT;
            bool adaptiveThreshold = true;
            double adaptiveValue = 0.6;
            static FasTextOptions getRecall(){
                FasTextOptions options;
                options.minProbabilityNM1=0;
                return options;
            }
            static FasTextOptions getSpeed(){
                FasTextOptions options;
                options.minProbabilityNM1=0.4;
                return options;
            }
        };

    }
}
#endif //UIC_FASTEXTOPTIONS_H
