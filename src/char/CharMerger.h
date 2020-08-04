//
// Created by fab on 30.01.17.
//

#ifndef UIC_CHARMERGER_H
#define UIC_CHARMERGER_H

#include "CharacterCandidate.h"

namespace cv {
    namespace text {

        class CharMerger {

            static int getDirection(uchar *scanLine, int cols);

            static double aspectRatio(const PCharacterCandidate &ch);

            static double aspectRatio(const Rect &r);

        public:
            static bool areLinked(const PCharacterCandidate &ch1, const PCharacterCandidate ch2);
            static void mergeChars(vector<PCharacterCandidate> &chars);
            static void mergeChars(deque<PCharacterCandidate> &chars);
        };
    }
}


#endif //UIC_CHARMERGER_H
