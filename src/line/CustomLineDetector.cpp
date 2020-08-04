//
// Created by fab on 15.11.16.
//

#include <opencv/cv.hpp>
#include "LineCandidate.h"
#include "CustomLineDetector.h"
#include "../ui/UicSelector.h"
#include "../uic/UicDebugHelper.h"

namespace cv {
    namespace text {
        using namespace uic;

        PLineVector CustomLineDetector::process(Mat &original, Mat &img, PCharVector characters) {
            PCharVector likelyCharacters = PCharVector(new CharVector());
            PCharVector unlikelyCharacters = PCharVector(new CharVector());

            for (auto ch:*characters) {
                if (ch->getIsCharProbability() >= minCharProbability)
                    likelyCharacters->push_back(ch);
                else
                    unlikelyCharacters->push_back(ch);
            }

            if (likelyCharacters->empty())
                return new vector<PTextLine>();
//            double angle = 0;
//            TimeLogger::logEnd("CustomLineDetector-findTextDirection");

//            TimeLogger::logStart("CustomLineDetector-applyRotation");
//            Mat rotationMatrix = getRotationMatrix2D(Point(0, 0), angle * 180 / CV_PI, 1);
//            applyRotation(rotationMatrix, characters);
//            TimeLogger::logEnd("CustomLineDetector-applyRotation");

//            TimeLogger::logStart("CustomLineDetector-createLineCandidates");


            LineSetTop topSortedLines;
            LineSetBottom bottomSortedLines;
            createLineCandidates(likelyCharacters, topSortedLines, bottomSortedLines);
//            TimeLogger::logEnd("CustomLineDetector-createLineCandidates");

//            TimeLogger::logStart("CustomLineDetector-addCharsToLine");
            priority_queue<PLineCandidate, LineCandidateVector, LineCandidate::SortByNumberOfLetters> sortedLines;
            addCharsToLines(topSortedLines, bottomSortedLines, sortedLines);
//            TimeLogger::logEnd("CustomLineDetector-addCharsToLine");

//            TimeLogger::logStart("CustomLineDetector-filterLines");
            PLineVector lines = filterLines(sortedLines,
                                            unlikelyCharacters);
//            TimeLogger::logEnd("CustomLineDetector-filterLines");

//            UicViewer::show("CustomLineDetector_chars", UicViewer::drawChars(characters, imgSize));
//            UicDebugHelper::show("CustomLineDetector_lines", UicDebugHelper::drawLines(lines, Size(img.cols,img.rows)));
//            waitKey();

            return lines;
        }

        void CustomLineDetector::createLineCandidates(const PCharVector &characters,
                                                      LineSetTop &topSorted,
                                                      LineSetBottom &bottomSorted) {
            for (PCharacterCandidate ch:*characters) {
                PLineCandidate lineCandidate(new LineCandidate(ch));
                topSorted.insert(lineCandidate);
                bottomSorted.insert(lineCandidate);
            }
        }

        double CustomLineDetector::getMainTextDirection(const PCharVector &candidates) {
            int ANGLE_COUNT = 180;
            int angles[ANGLE_COUNT];
            memset(angles, 0, ANGLE_COUNT * sizeof(int));
            int maxAngleIndex = -1;

            for (int i = 0; i < candidates->size(); i++) {
                for (int j = i + 1; j < candidates->size(); j++) {

                    if (couldBeOnSameLine((*candidates)[i], (*candidates)[j])) {
                        Point d = (*candidates)[j]->center - (*candidates)[i]->center;

                        double angle = atan2(d.y, d.x);
                        if (angle < 0)
                            angle += CV_PI;
                        else if (angle > CV_PI)
                            angle -= CV_PI;
                        int angleIndex = ((int) round(angle / CV_PI * ANGLE_COUNT)) % ANGLE_COUNT;
                        angles[angleIndex]++;
                        if (maxAngleIndex == -1 || angles[angleIndex] > angles[maxAngleIndex]) {
                            maxAngleIndex = angleIndex;
                        }
                    }
                }

            }
            double angle = maxAngleIndex * CV_PI / ANGLE_COUNT;
            if (angle > CV_PI / 2)
                angle -= CV_PI;
            return angle;
        }

        bool
        CustomLineDetector::couldBeOnSameLine(const PCharacterCandidate &ch1, const PCharacterCandidate &ch2) {
            int maxHeight = MAX(ch1->height, ch2->height);
            int minHeight = MIN(ch1->height, ch2->height);
            if (minHeight < maxHeight * 0.75)
                return false;
//            return true;

            int intersection = (ch1->rect & ch2->rect).area();
            if (intersection > 0.5 * MIN(ch1->rect.area(), ch2->rect.area()))
                return false;

            double d = norm(ch1->center - ch2->center);
            return d < maxHeight * 2;
        }

        void CustomLineDetector::applyRotation(Mat rotation, PCharVector &characters) {
            for (auto ch:*characters) {
                vector<Point2f> point;
                point.push_back(ch->center);
                cv::transform(point, point, rotation);
                Point rotatedPoint(point[0]);
                ch->setLineDistance(rotatedPoint);
            }
        }

        void CustomLineDetector::addCharsToLines(LineSetTop &topSorted,
                                                 LineSetBottom &bottomSorted,
                                                 priority_queue<PLineCandidate, LineCandidateVector, LineCandidate::SortByNumberOfLetters> &sortedLines) {
            auto itTopSorted = topSorted.begin();
            auto itBottomSorted = bottomSorted.begin();

            unordered_set<Ptr<LineCandidate>> activeLines;
            double topDistance = (*itTopSorted)->characterCandidate->getTop();
            double bottomDistance = (*itBottomSorted)->characterCandidate->getBottom();
            while (itBottomSorted != bottomSorted.end()) {
                if (itTopSorted != topSorted.end() && topDistance < bottomDistance) {
                    activeLines.insert(*itTopSorted);
                    itTopSorted++;
                    if (itTopSorted != topSorted.end())
                        topDistance = (*itTopSorted)->characterCandidate->getTop();
                } else {
                    PLineCandidate bottom = *itBottomSorted;
                    activeLines.erase(bottom);
                    for (PLineCandidate c:activeLines) {
                        c->addLetterIfMatching(bottom->characterCandidate);
                        bottom->addLetterIfMatching(c->characterCandidate);
                    }
                    bottom->filterCharacters();
                    sortedLines.push(bottom);
                    itBottomSorted++;
                    if (itBottomSorted != bottomSorted.end())
                        bottomDistance = (*itBottomSorted)->characterCandidate->getBottom();
                }
            }
        }

        PLineVector CustomLineDetector::filterLines(
                priority_queue<PLineCandidate, LineCandidateVector, LineCandidate::SortByNumberOfLetters> sortedLines,
                PCharVector unlikelyCharacters) {
            unordered_set<int> usedCharacters;
            PLineVector textLines = new LineVector();
            int cnt=0;

            while (!sortedLines.empty() && sortedLines.top()->size() >= 2) {
                PLineCandidate line = sortedLines.top();
                sortedLines.pop();

//                bool changed = false;
//                for(auto it=unlikelyCharacters->begin() ; it < unlikelyCharacters->end(); it++ ){
//                    auto ch = *it;
//                    if(line->isGoodMatch(ch)){
//                        line->addLetter(ch);
//                        changed=true;
//                        unlikelyCharacters->erase(it);
//                        cnt++;
//                    }
//                }
//                if(changed)
//                    line->filterCharacters();

                if (!line->contains(usedCharacters)) {
//                    for (auto &ch:line->getCharacters()) {
//                        usedCharacters[ch->id] = true;
//                    }
                    line->splitInto(textLines, usedCharacters);
                } else {
//                        sortedLines.push(line);
                }
            }
            return textLines;
        }
    }
}