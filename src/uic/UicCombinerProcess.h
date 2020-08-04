//
// Created by fab on 18.06.17.
//

#ifndef UIC_UICCOMBINERPROCESS_H
#define UIC_UICCOMBINERPROCESS_H

#include "UicDetector.h"

namespace cv {
    namespace uic {
        class UicCombinerProcess : public WrapperImageProcess<PUicResultVector> {
        public:
            UicCombinerProcess(PUicProcess delegate) {
                this->setChild(delegate);
            }

            inline PUicResultVector processImage(Mat original, Mat img) {
                PUicResultVector results = child->processImage(original, img);
//                if (results->size() > 0)
//                    combine(results);
                return results;
            }

            void combine(PUicResultVector uics) {
                auto it = uics->begin();
                PUicResult best = *it++;

                for (; it < uics->end(); it++) {
                    PUicResult &current = *it;
                    combine(best->lines[0], current->lines[0]);
                    combine(best->lines[1], current->lines[1]);
                    combine(best->lines[2], current->lines[2]);
                }
            }

            bool areSameChar(PCharacterCandidate &c1, PCharacterCandidate &c2) {
                return (c1->rect & c2->rect).area() / (double) (c1->rect | c2->rect).area() > 0.8;
            }

            void combine(PTextLine &line, PTextLine &other) {
                if ((line->rect & other->rect).width > 0) {
                    PCharVector &lineChars = line->getCharacters();
                    PCharVector &otherChars = other->getCharacters();
                    auto itLine = lineChars->begin();
                    auto itOther = otherChars->begin();

                    while (itLine != lineChars->end() && itOther != otherChars->end()) {
                        PCharacterCandidate &lineChar = *itLine;
                        PCharacterCandidate &otherChar = *itOther;
                        if (areSameChar(lineChar, otherChar)) {
                            if (otherChar->getConfidence() > lineChar->getConfidence()) {
                                if (otherChar->getPrediction() != lineChar->getPrediction())
                                    cout << line->getPrediction() << " '" << lineChar->getPrediction() << "-"
                                         << otherChar->getPrediction() << "' " << endl;
                                lineChar->setPrediction(otherChar->getPrediction(), otherChar->getConfidence());
                            }
                            itLine++;
                            itOther++;
                        } else if (lineChar->rect.x > otherChar->rect.x) {
                            itOther++;
                        } else {
                            itLine++;
                        }
                    }
                }
            }

            string getProcessName() {
                return this->child->toString() + " >> UicCombiner";
            }
        };
    }
}
#endif //UIC_UICCOMBINERPROCESS_H
