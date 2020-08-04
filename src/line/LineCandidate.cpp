//
// Created by fab on 10.11.16.
//
#include "LineCandidate.h"
#include <opencv/cv.hpp>
#include <iostream>

using namespace std;
namespace cv {
    namespace text {
        double LineCandidate::HEIGHT_SIMILARITY = 0.8;
        double LineCandidate::COLOR_SIMILARITY = 80;
        double LineCandidate::CENTER_SIMILARITY = 3;
        double LineCandidate::OVERLAP_THRESHOLD = 0.2;
        double LineCandidate::MAX_DISTANCE = 2;
        double LineCandidate::MAX_HEIGHT_DIFF = 0.1;
        double LineCandidate::MAX_COLOR_DIFF = 10;

        LineCandidate::LineCandidate(const Ptr<CharacterCandidate> &characterCandidate)
                : characterCandidate(characterCandidate) {
//            characters.insert(characterCandidate);
            charsLeftSorted.insert(characterCandidate);
            lineIntersectionMin = characterCandidate->getTop();
            lineIntersectionMax = characterCandidate->getBottom();
            maxX = characterCandidate->getRight();
            minX = characterCandidate->getLeft();
            averageWidth = characterCandidate->width;
            averageHeight = characterCandidate->width;
            coveredWidth = characterCandidate->width;
        }

        void LineCandidate::addLetterIfMatching(const Ptr<CharacterCandidate> &toAdd) {
            if (isSimilar(toAdd) && isCentered(toAdd) &&
                (characterCandidate.empty() || toAdd->getLeft() > characterCandidate->getLeft()))
                charsLeftSorted.insert(toAdd);
        }

        void LineCandidate::addLetter(const Ptr<CharacterCandidate> &toAdd) {
            charsLeftSorted.insert(toAdd);
        }

        void LineCandidate::filterCharacters() {
            if (charsLeftSorted.size() < 2)
                return;
            characters.clear();
            auto itLeft = charsLeftSorted.begin();

//            characters.push_front(characterCandidate);
//            const PCharacterCandidate *next = findNextNeighbour(nullptr, *itLeft, ++itLeft);
            auto first = *itLeft;
            findNextNeighbor(nullptr, first, ++itLeft);
//            if (next) {d
        }

        const PCharacterCandidate *
        LineCandidate::findNextNeighbor(const PCharacterCandidate *last, const PCharacterCandidate &current,
                                        set<cv::Ptr<cv::text::CharacterCandidate>, cv::text::SortCharByTransformedLeft>::iterator &itLeft) {
            if (itLeft == charsLeftSorted.end()) {
                if (last && areClose(*last, current)) {
                    characters.push_front(current);
                    return &current;
                } else {
                    return nullptr;
                }
            }

            const PCharacterCandidate &possibleNext = *itLeft++;

            bool areSeparated = getOverlap(current, possibleNext) <= OVERLAP_THRESHOLD;
            if (areSeparated) {
                const PCharacterCandidate *actualNext = findNextNeighbor(&current, possibleNext, itLeft);
                if (last && areClose(*last, current, MAX_DISTANCE) ||
                    actualNext && areClose(current, *actualNext, MAX_DISTANCE)) {
                    // Current has a neighbour. We can add it.
                    characters.push_front(current);
                    return &current;
                } else {
                    // Current has no close neighbour
                    return actualNext;
                }
            } else if (hasBetterMatch(last, current, possibleNext)) {
                // Replace current with this character
                return findNextNeighbor(last, possibleNext, itLeft);
            } else {
                // Try next character
                return findNextNeighbor(last, current, itLeft);
            }
        }

        bool LineCandidate::isSimilar(const Ptr<CharacterCandidate> &toAdd) const {
            if (getSizeSimilarity(characterCandidate, toAdd) < HEIGHT_SIMILARITY)
                return false;
            return true;
//            return norm(characterCandidate->getMean() - toAdd->getMean()) <= COLOR_SIMILARITY;
//            1.00, 37.01, 62.12, 45.37, 54.70, 76.82, 38, 21

        }

        bool LineCandidate::isGoodMatch(const Ptr<CharacterCandidate> &toAdd) const {
            double offset = abs(characterCandidate->rect.y - toAdd->rect.y) +
                            abs(characterCandidate->rect.br().y - toAdd->rect.br().y);
            if (offset / characterCandidate->rect.height > 0.1)
                return false;
            return isSimilar(toAdd);
        }

        bool LineCandidate::hasBetterMatch(const PCharacterCandidate *last, const PCharacterCandidate &current,
                                           const PCharacterCandidate &candidate) {
            double sizeDiff =
                    getSizeSimilarity(characterCandidate, candidate) - getSizeSimilarity(characterCandidate, current);
            if (abs(sizeDiff) > MAX_HEIGHT_DIFF)
                return sizeDiff > 0;
            double colorDiff =
                    norm(candidate->getMean() - characterCandidate->getMean()) -
                    norm(current->getMean() - characterCandidate->getMean());
            if (abs(colorDiff) > MAX_COLOR_DIFF)
                return colorDiff < 0;

            return candidate->getIsCharProbability() > current->getIsCharProbability();
        }

        unsigned long LineCandidate::size() {
            return characters.size();
        }

        bool LineCandidate::isValidLine() {
            return lineIntersectionMin < lineIntersectionMax;
        }

//        set<Ptr<CharacterCandidate>, SortCharByTransformedX> &LineCandidate::getCharacters() {
//            return characters;
//        }

        bool LineCandidate::isCentered(const Ptr<CharacterCandidate> &toAdd) {
            return abs(characterCandidate->getLineDistanceY() - toAdd->getLineDistanceY()) <
                   characterCandidate->height / CENTER_SIMILARITY;
        }

        float LineCandidate::getOverlap(const Ptr<CharacterCandidate> &left, const Ptr<CharacterCandidate> &right) {
            float minArea = MIN(left->rect.area(), right->rect.area());
            float overlappingArea = (left->rect & right->rect).area();
            return overlappingArea / minArea;
        }

        bool LineCandidate::contains(unordered_set<int> &usedCharacters) {
            for (auto &ch: charsLeftSorted) {
                if (usedCharacters.find(ch->id) != usedCharacters.end())
                    return true;
            }
            return false;
        }


        float LineCandidate::getCoveringRatio() {
            return coveredWidth / (maxX - minX);
        }

        void LineCandidate::splitInto(PLineVector &out, unordered_set<int> &usedCharacters) {
            auto it = characters.begin();
            PTextLine current = new TextLine();
            PCharacterCandidate *last = nullptr;
            while (it != characters.end()) {
                if (last && !areClose(*last, *it, 3)) {
//                    assert(current.size() > 1);
                    if (current->size() > 1) {
                        out->push_back(current);
                    }
                    current = new TextLine();
                }
                current->addChar(*it);
                last = &*it;
                it++;
            }
            for (auto &ch:characters) {
                usedCharacters.insert(ch->id);
            }
//            assert(current.size() > 1);
            if (current->size() > 1) {
                out->push_back(current);
            }
        }

        float LineCandidate::getRelativeCoveredWidth() {
            return coveredWidth / averageWidth;
        }

        void LineCandidate::drawLine(Mat &img) {
            Point l1, l2;
            for (auto ch:characters) {
                l2 = ch->center;
                if (l1.x != 0 && l1.y != 0) {
                    arrowedLine(img, l1, l2, Scalar(255, 0, 0), 1);
                }
                rectangle(img, ch->getRect().tl(), ch->getRect().br(), Scalar(255, 0, 0));
                l1 = l2;
            }
        }


        bool
        LineCandidate::areClose(const PCharacterCandidate &left, const PCharacterCandidate &right, double maxDistance) {
//            int height = MIN(left->height, right->height);
            return abs(right->getLeft() - left->getLineDistanceX()) < characterCandidate->height * maxDistance;
        }

        double LineCandidate::getSizeSimilarity(const PCharacterCandidate &c1, const PCharacterCandidate &c2) const {
            return MIN(c1->height, c2->height)
                   / (double) MAX(c1->height, c2->height);
        }


        bool ERStatLineSorter::operator()(LineCandidate *lhs, LineCandidate *rhs) {
            double d1 = getDistance(*lhs);
            double d2 = getDistance(*rhs);
            return d1 < d2;
        }

        double ERStatLineSorter::getDistance(LineCandidate &lc) {
            return lc.characterCandidate->getLineDistanceY();
        }

        double ERStatTopLineSorter::getDistance(LineCandidate &lc) {
            return lc.characterCandidate->getTop();
        }

        double ERStatBottomLineSorter::getDistance(LineCandidate &lc) {
            return lc.characterCandidate->getBottom();
        }

    }
}