//
// Created by fab on 05.03.17.
//

#ifndef UIC_DETECTOREVALUATOR_H
#define UIC_DETECTOREVALUATOR_H

#include "../char/CharacterCandidate.h"

using namespace cv::text;
using namespace cv;

class DetectorEvaluator {
    struct CharExtractor {
        virtual PCharVector getChars(Mat img)=0;
    };
};


#endif //UIC_DETECTOREVALUATOR_H
