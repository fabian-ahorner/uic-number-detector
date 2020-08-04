//
// Created by fab on 02.04.17.
//

#ifndef UIC_UICCHARDETECTOR_H
#define UIC_UICCHARDETECTOR_H

#include <CharacterDetector.h>
#include <LineDetector.h>
#include "uic/UicDetector.h"

namespace cv {
    namespace text {
        using namespace uic;

        class UicCharacterDetector : public CharProcess {
        private:
            PUicProcess detector;
        public:
            inline UicCharacterDetector(PUicProcess detector) {
                this->detector = detector;
            }
            inline PCharVector processImage(Mat original, Mat img) {
                UicResultVector uics = *detector->processImage(original, img);
                PCharVector characters = new vector<PCharacterCandidate>();
                if (uics.size() > 0) {
                    CharVector &newCharacters = *uics[0]->lines[0]->getCharacters();
                    characters->insert(std::end(*characters), std::begin(newCharacters), std::end(newCharacters));
                    newCharacters = *uics[0]->lines[1]->getCharacters();
                    characters->insert(std::end(*characters), std::begin(newCharacters), std::end(newCharacters));
                    newCharacters = *uics[0]->lines[2]->getCharacters();
                    characters->insert(std::end(*characters), std::begin(newCharacters), std::end(newCharacters));
                }
                return characters;
            }

            inline string getProcessName() {
                return "UicCharacterDetector(" + detector->toString() + ")";
            }
        };
    }
}
#endif //UIC_UICCHARDETECTOR_H
