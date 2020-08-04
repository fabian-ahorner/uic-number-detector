//
// Created by fab on 10.11.16.
//

#ifndef UIC_LINECANDIDATE_H
#define UIC_LINECANDIDATE_H

#include "../char/CharacterCandidate.h"
#include "TextLine.h"
#include <set>
#include <opencv2/core/core.hpp>
#include <unordered_set>

namespace cv {
    namespace text {
        using namespace std;

        class LineCandidate {
        private:
            set<Ptr<CharacterCandidate>, SortCharByTransformedLeft> charsLeftSorted;
//            set<Ptr<CharacterCandidate>, SortCharByTransformedX> characters;
            deque<Ptr<CharacterCandidate>> characters;
            double lineIntersectionMin,
                    lineIntersectionMax;
            float coveredWidth;
            float averageWidth = 0;
            float averageHeight = 0;
            int maxX, minX;
        public:
            static double HEIGHT_SIMILARITY;
            static double CENTER_SIMILARITY;
            static double COLOR_SIMILARITY;
            static double OVERLAP_THRESHOLD;
            static double MAX_DISTANCE;
            static double MAX_HEIGHT_DIFF;
            static double MAX_COLOR_DIFF;
            Mat debugImage;
            const Ptr<CharacterCandidate> characterCandidate;

            LineCandidate(const Ptr<CharacterCandidate> &characterCandidate);

            float getCoveringRatio();

            void addLetterIfMatching(const Ptr<CharacterCandidate> &toAdd);
            void addLetter(const Ptr<CharacterCandidate> &toAdd);

            bool operator==(const LineCandidate &other) const {
                return (characterCandidate->id == other.characterCandidate->id);
            }

            bool operator<(const LineCandidate &other) const {
                return getLineDistance() < other.getLineDistance();
            }

            inline double getLineDistance() const {
                return characterCandidate->getLineDistanceY();
            }

            unsigned long size();

            bool isValidLine();

//            set<Ptr<CharacterCandidate>, SortCharByTransformedX> &getCharacters();

            bool isSimilar(const Ptr<CharacterCandidate> &toAdd) const;
            bool isGoodMatch(const Ptr<CharacterCandidate> &toAdd) const;

            bool isCentered(const Ptr<CharacterCandidate> &toAdd);

            static bool sortByCoveredWidth(LineCandidate *l1, LineCandidate *l2) {
                return l1->coveredWidth > l2->coveredWidth;
            }

            static bool sortByLineTop(LineCandidate *l1, LineCandidate *l2) {
                return l1->characterCandidate->getTop() < l2->characterCandidate->getTop();
            }

            static bool sortByLineBottom(LineCandidate *l1, LineCandidate *l2) {
                return l1->characterCandidate->getBottom() < l2->characterCandidate->getBottom();
            }

            float getOverlap(const Ptr<CharacterCandidate> &left, const Ptr<CharacterCandidate> &right);

//            float updateCoveredWidth(bool usedCharacters[]);

            void splitInto(PLineVector &out, unordered_set<int> &usedCharacters);

            float getRelativeCoveredWidth();

//            void removeInvalidCharacters();

            void drawLine(Mat &mat);

            void addAndUpdateStatistics(const Ptr<CharacterCandidate> &add);

            void filterCharacters();

            const PCharacterCandidate *
            findNextNeighbor(const PCharacterCandidate *last, const PCharacterCandidate &current,
                             set<cv::Ptr<cv::text::CharacterCandidate>, cv::text::SortCharByTransformedLeft>::iterator &itLeft);

            bool hasBetterMatch(const PCharacterCandidate *pPtr, const PCharacterCandidate &current,
                                const PCharacterCandidate &ch);

            bool contains(unordered_set<int> &usedCharacters);

            bool areClose(const PCharacterCandidate &left, const PCharacterCandidate &right, double maxDistance = 1.5);

            double getSizeSimilarity(const PCharacterCandidate &c1, const PCharacterCandidate &c2) const;

            struct SortByNumberOfLetters {
                inline bool operator()(const Ptr<LineCandidate> &l1, const Ptr<LineCandidate> &l2) {
                    if (l1->size() > l2->size())
                        return false;
                    if (l1->size() < l2->size())
                        return true;
                    for (int i = 0; i < l1->size(); i++) {
                        if (l1->characters[i]->getCharQuality() > l2->characters[i]->getCharQuality())
                            return false;
                        if (l1->characters[i]->getCharQuality() < l2->characters[i]->getCharQuality())
                            return true;
                    }
                    return false;
                }
            };
        };


        struct SortLinesByCoveredWidth {
            inline bool operator()(const Ptr<LineCandidate> &l1, const Ptr<LineCandidate> &l2) {
                return l1->getRelativeCoveredWidth() < l2->getRelativeCoveredWidth();
            }
        };

        typedef Ptr<LineCandidate> PLineCandidate;
        typedef vector<PLineCandidate> LineCandidateVector;

        struct SortLinesByTop {
            inline bool operator()(const PLineCandidate &c1, const PLineCandidate &c2) {
                int diff = c1->characterCandidate->getTop() - c2->characterCandidate->getTop();
                if (diff == 0)
                    return c1->characterCandidate->id < c2->characterCandidate->id;
                return diff < 0;
            }
        };

        struct SortLinesByBottom {
            inline bool operator()(const PLineCandidate &c1, const PLineCandidate &c2) {
                int diff = c1->characterCandidate->getBottom() - c2->characterCandidate->getBottom();
                if (diff == 0)
                    return c1->characterCandidate->id < c2->characterCandidate->id;
                return diff < 0;
            }
        };

        class ERStatLineSorter {
        private:
            Point p1, p2;
        public:
            inline ERStatLineSorter(Point p1, Point p2) :
                    p1(p1), p2(p2) {}

            virtual double getDistance(LineCandidate &er);

            bool operator()(LineCandidate *lhs, LineCandidate *rhs);
        };

        class ERStatTopLineSorter : public ERStatLineSorter {
        public:
            inline ERStatTopLineSorter(Point p1, Point p2) :
                    ERStatLineSorter(p1, p2) {}

            double getDistance(LineCandidate &er);
        };

        class ERStatBottomLineSorter : public ERStatLineSorter {
        public:
            inline ERStatBottomLineSorter(Point p1, Point p2) :
                    ERStatLineSorter(p1, p2) {}

            double getDistance(LineCandidate &er);
        };


    }
}
namespace std {
    template<>
    struct hash<cv::text::PLineCandidate> {
        std::size_t operator()(const cv::text::PLineCandidate l) const {
            return (std::hash<void *>()(&(*l)));
        }
    };
}

#endif //UIC_LINECANDIDATE_H
