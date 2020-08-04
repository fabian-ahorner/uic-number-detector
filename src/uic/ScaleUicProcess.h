//
// Created by fab on 12.03.17.
//

#ifndef UIC_SCALEUICPROCESS_H
#define UIC_SCALEUICPROCESS_H

#include "ScaleWrapper.h"
#include "CharacterCandidate.h"
#include "CharacterDetector.h"
#include "ScaledCharacter.h"
#include "UicDetector.h"
#include "../line/ScaleLineProcess.h"

namespace cv {
    namespace uic {
        class ScaleUicProcess : public ScaleWrapper<PUicResult> {
        public:
            ScaleUicProcess(const PUicProcess &delegate, double scale = 1.6, const Size &minSize = Size(1, 1))
                    : ScaleWrapper(delegate, scale, minSize) {
            }

            PUicResult scaleOut(const PUicResult &uicIn, const Size &currentSize, const Size &originalSize) {
                const PTextLine &line1 = ScaleLineProcess::scaleLine(uicIn->lines[0], currentSize, originalSize);
                const PTextLine &line2 = ScaleLineProcess::scaleLine(uicIn->lines[1], currentSize, originalSize);
                const PTextLine &line3 = ScaleLineProcess::scaleLine(uicIn->lines[2], currentSize, originalSize);
                return new UicResult(line1, line2, line3);
            }
            virtual PUicResultVector processImage(Mat original, Mat img) {
                PUicResultVector out = ScaleWrapper::processImage(original,img);
                std::sort(out->begin(), out->end(), UicDetector::sortByScore);
                return out;
            }
        };
    }
}

#endif //UIC_SCALEUICPROCESS_H
