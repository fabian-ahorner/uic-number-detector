//
// Created by fab on 12.03.17.
//

#ifndef UIC_SCALELINEPROCESS_H
#define UIC_SCALELINEPROCESS_H


#include "ScaleWrapper.h"
#include "CharacterCandidate.h"
#include "CharacterDetector.h"
#include "ScaledCharacter.h"
#include "LineDetector.h"

namespace cv {
    namespace text {
        class ScaleLineProcess : public ScaleWrapper<PTextLine> {
        public:
            ScaleLineProcess(const PLineProcess &delegate, double scale = 1.6, const Size &minSize = Size(1, 1))
                    : ScaleWrapper(delegate, scale, minSize) {
            }
            static PTextLine scaleLine(const PTextLine &in, const Size &currentSize, const Size &originalSize) {
                PCharVector scaledChars = ScaledCharacter::from(in->getCharacters(), currentSize,
                                                                originalSize);
                PTextLine out=new TextLine(scaledChars);
                out->setPrediction(in->getPrediction());
                return out;
            }
            PTextLine scaleOut(const PTextLine &in, const Size &currentSize, const Size &originalSize) {
                return scaleLine(in, currentSize, originalSize);
            }
        };
    }
}

#endif //UIC_SCALELINEPROCESS_H
