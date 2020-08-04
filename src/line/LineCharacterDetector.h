//
// Created by fab on 05.03.17.
//

#ifndef UIC_LINECHARACTERDETECTOR_H
#define UIC_LINECHARACTERDETECTOR_H

#include "LineDetector.h"

namespace cv {
    namespace text {
        static int n = 0;

        class LineCharacterDetector : public CharProcess {
        private:
            PLineProcess detector;
        public:
            inline LineCharacterDetector(PLineProcess detector) {
                this->detector = detector;
            }

            inline PCharVector processImage(Mat original, Mat img) {
                PLineVector lines = detector->processImage(original, img);
//                UicDebugHelper::show("Characters", UicDebugHelper::drawSeperateLines(lines,Size(img.cols, img.rows)));
//                waitKey();
//                UicDebugHelper::save(UicDebugHelper::drawSeperateLines(lines, Size(img.cols, img.rows)));


                PCharVector characters = new vector<PCharacterCandidate>();
                for (auto &line:*lines) {
                    PCharVector &newCharacters = line->getCharacters();
                    characters->insert(std::end(*characters), std::begin(*newCharacters), std::end(*newCharacters));
                }
                return characters;
            }

            inline string getProcessName() {
                return "LineCharacterDetector(" + detector->toString() + ")";
            }
        };
    }
}
#endif //UIC_LINECHARACTERDETECTOR_H
