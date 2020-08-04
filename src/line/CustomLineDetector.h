//
// Created by fab on 15.11.16.
//

#ifndef UIC_CUSTOMLINEDETECTOR_H
#define UIC_CUSTOMLINEDETECTOR_H

#include "LineDetector.h"
#include <queue>
#include <set>
#include "LineCandidate.h"
#include "LineDetector.h"
#include <queue>
#include <set>
#include <unordered_set>

namespace cv {
    namespace text {

//        using LineSetTop = set<PLineCandidate, SortLinesByTop>;
        typedef set<PLineCandidate, SortLinesByTop> LineSetTop;
        typedef set<PLineCandidate, SortLinesByBottom> LineSetBottom;

        class CustomLineDetector : public LineDetector {
        private:
            double minCharProbability = 0;
            double getMainTextDirection(const PCharVector &candidates);

            bool couldBeOnSameLine(const Ptr<CharacterCandidate> &ch1, const Ptr<CharacterCandidate> &ch2);

            void applyRotation(Mat rotation, PCharVector &characters);

            void createLineCandidates(const PCharVector &candidates,
                                      LineSetTop &topSorted,
                                      LineSetBottom &bottomSorted);

            PLineVector process(Mat& original,Mat &img, PCharVector characters);

            void addCharsToLines(LineSetTop &topSorted,
                                 LineSetBottom &bottomSorted,
                                 priority_queue<PLineCandidate, LineCandidateVector, LineCandidate::SortByNumberOfLetters> &sortedLines);

        public:
            CustomLineDetector(PCharProcess charDetector) :
                    LineDetector(charDetector) {
            }


            PLineVector filterLines(
                    priority_queue<PLineCandidate, LineCandidateVector, LineCandidate::SortByNumberOfLetters> sortedLines,
                    PCharVector ptr);

            inline string getProcessName(){
                return "CustomLineDetector";
            }
        };
    }
}


#endif //UIC_CUSTOMLINEDETECTOR_H
