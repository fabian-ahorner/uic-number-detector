//
// Created by fab on 18.06.17.
//

#include "MovedCharacter.h"

namespace cv {
    namespace text {
        PCharVector MovedCharacter::from(PCharVector &pChars, Point move) {
            PCharVector pMovedChars = new vector<PCharacterCandidate>(pChars->size());
            CharVector &chars = *pChars;
            CharVector &movedChars = *pMovedChars;
            for (int i = 0; i < pChars->size(); ++i) {
                movedChars[i] = new MovedCharacter(chars[i], move);
                movedChars[i]->setPrediction(chars[i]->getPrediction(), chars[i]->getConfidence());
            }
            return pMovedChars;
        }
    }
}