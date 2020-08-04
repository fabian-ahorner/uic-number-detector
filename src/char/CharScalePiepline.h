//
// Created by fab on 07.03.17.
//

#ifndef UIC_SCALECHARPROCESS_H
#define UIC_SCALECHARPROCESS_H

#include "ScaleWrapper.h"
#include "CharacterCandidate.h"
#include "CharacterDetector.h"
#include "ScaledCharacter.h"

namespace cv {
    namespace text {
        class ScaleCharProcess : public ScaleWrapper<PCharacterCandidate> {
        public:
            ScaleCharProcess(const PCharProcess &delegate, double scale = 1.6, const Size &minSize = Size(1, 1))
                    : ScaleWrapper(delegate, scale, minSize) {
            }

            PCharacterCandidate scaleOut(const PCharacterCandidate &in, const Size &currentSize, const Size &originalSize) {
                return new ScaledCharacter(in, currentSize, originalSize);
            }
        };
    }
}
#endif //UIC_SCALECHARPROCESS_H
