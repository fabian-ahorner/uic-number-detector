//
// Created by fab on 18.06.17.
//

#include "FeedbackLoopProcess.h"

namespace cv {
    namespace uic {
        PTextLine FeedbackLoopProcess::moveLine(PTextLine line, Point move) {
            PCharVector movedChars = MovedCharacter::from(line->getCharacters(), move);
            PTextLine out = new TextLine(movedChars);
            out->setPrediction(line->getPrediction());
            return out;
        }
    }
}