//
// Created by fab on 26.11.16.
//

#ifndef UIC_FPOINTDETECTOR_H
#define UIC_FPOINTDETECTOR_H

//#define UIC_FPOINTDETECTOR_DEBUG 1


#include <opencv2/core/core.hpp>
#include <queue>
#include "FastPoint.h"
#include "NeighbourVisitor.h"
#include "FastPointFactory.h"

using namespace std;
namespace cv {
    namespace text {
        template<class VAL>
        class FastPointHelper {
            Scalar otherDiff;
            const VAL *spotlight;
            double minDistance;
            VAL minDistanceValue;
            const double threshold;
            double curThreshold;
            bool hasOtherValue = false;

            bool loadValue(int index);

        public:
            const NeighbourVisitor<VAL> neighbourVisitor;
            const NeighbourVisitor<VAL> connectivityVisitor;
            const Ptr<FastPointFactory<VAL>> factory;
            const double adaptiveValue;
            bool isSame[12], adaptiveThreshold;
            VAL spotlightValue;

            inline FastPointHelper(Ptr<FastPointFactory<VAL>> factory, const Mat &img, double threshold,
                                   bool adaptiveThreshold, double adaptiveValue)
                    : factory(factory), neighbourVisitor(createNeighbourVisitor<VAL>(img, NeighbourVisitor_CIRCLE12)),
                      connectivityVisitor(createNeighbourVisitor<VAL>(img, NeighbourVisitor_CIRCLE12_CONNECTIONS)),
                      threshold(threshold), adaptiveThreshold(adaptiveThreshold), adaptiveValue(adaptiveValue) {}


            void moveTo(const VAL *spotlight);

            bool loadAndCheckValues();

            inline double getMinDistance() {
                return minDistance;
            }

            inline VAL &getMinDistanceValue() {
                return minDistanceValue;
            }

            bool isConnectedToCenter(int index);

            bool isSameAsCenter(const VAL &value);
        };


        template<class VAL>
        class FastPointDetector {
            static const int BORDER = 2;
            FastPointHelper<VAL> helper;
//            NeighbourVisitor<Vec3b> neighbourVisitor;
            int curCol, curRow;
//            int threshold;
            const Mat img;
            const bool nonMaxSuppression;
            int lastId = 1;
            vector<Ptr<FastPoint<VAL>>> points;

            bool checkSpotlightForKeypoint(const VAL *spotlight);

            const Ptr<FastPointFactory<VAL>> factory;
        public:
#ifdef UIC_FPOINTDETECTOR_DEBUG
            Mat debug;
#endif

            FastPointDetector(const Ptr<FastPointFactory<VAL>> factory, Mat img, double threshold,
                              bool nonMaxSuppression = false, bool adaptiveThreshold = false,
                              double adaptiveValue = 0.5);

            ~FastPointDetector();

            vector<Ptr<FastPoint<VAL>>> detectKeypoints();

            int *neighbourCache[3];

            void suppressNonMax();

        };

        template<class VAL>
        FastPointDetector<VAL>::FastPointDetector(const Ptr<FastPointFactory<VAL>> factory, Mat img, double threshold,
                                                  bool nonMaxSuppression, bool adaptiveThreshold, double adaptiveValue)
                : factory(factory), img(img), helper(factory, img, threshold, adaptiveThreshold, adaptiveValue),
                  nonMaxSuppression(nonMaxSuppression) {
//            helper =;
            if (nonMaxSuppression) {
                for (int i = 0; i < 3; ++i) {
                    neighbourCache[i] = new int[img.cols];
                    memset(neighbourCache[i], 0, img.cols * sizeof(int));
                }
            }
#ifdef UIC_FPOINTDETECTOR_DEBUG
            debug = Mat::zeros(img.rows, img.cols, CV_8UC3);
#endif
        }

        template<class VAL>
        FastPointDetector<VAL>::~FastPointDetector() {
            if (nonMaxSuppression) {
                for (int i = 0; i < 3; ++i) {
                    if (neighbourCache[i]) {
                        delete neighbourCache[i];
                        neighbourCache[i] = nullptr;
                    }
                }
            }
        }

        template<class VAL>
        vector<Ptr<FastPoint<VAL>>> FastPointDetector<VAL>::detectKeypoints() {
//            this->threshold = threshold;
            for (curRow = BORDER; curRow < img.rows - BORDER; curRow++) {
                const VAL *spotlight = img.ptr<VAL>(curRow) + BORDER;
                for (curCol = BORDER; curCol < img.cols - BORDER; ++curCol) {
                    if (checkSpotlightForKeypoint(spotlight) && nonMaxSuppression) {
                        suppressNonMax();
                    }
                    spotlight++;
                }
                if (nonMaxSuppression) {
                    int *tmp = neighbourCache[2];
                    neighbourCache[2] = neighbourCache[1];
                    neighbourCache[1] = neighbourCache[0];
                    neighbourCache[0] = tmp;
                    memset(neighbourCache[0], 0, img.cols * sizeof(int));
                }
            }
#ifdef UIC_FPOINTDETECTOR_DEBUG
            namedWindow("FPoindDetector_debug", CV_WINDOW_NORMAL);
            imshow("FPoindDetector_debug", debug);
//            waitKey(0);
#endif
            return points;
        }

        template<class VAL>
        void FastPointDetector<VAL>::suppressNonMax() {
            Ptr<FastPoint<VAL>> &point = points.back();
            neighbourCache[0][curCol] = (int) points.size();
            for (int row = 2; row >= 0; row--) {
                for (int col = curCol - 1; col <= curCol + 1; col++) {
                    if (row == 0 && curCol == col)
                        break;
                    int index = neighbourCache[row][col];
                    if (index != 0) {
                        Ptr<FastPoint<VAL>> &pointB = points[index - 1];
                        if (!pointB->suppressed) {
                            if (point->diff > pointB->diff) {
                                pointB->suppressed = true;
                            } else {
                                point->suppressed = true;
                            }
                        }
                    }
                }
            }
        }

        template<class VAL>
        bool FastPointDetector<VAL>::checkSpotlightForKeypoint(const VAL *spotlight) {
            int MAX_SEGMENTS = 5;
            int segmentCount = 0;

            int lastSegmentSize = 0;
            bool wasSame = false;

            helper.moveTo(spotlight);
            if (!helper.loadAndCheckValues())
                return false;
            for (int i = 0; i < helper.neighbourVisitor.size; i++) {
                bool isSame = helper.isSame[i];
                if (isSame && !helper.isConnectedToCenter(i))
                    return false;

                bool hasChanged = wasSame != isSame;
                if (hasChanged) {
                    wasSame = isSame;
                    if (i != 0) {
                        segmentCount++;
                        if (segmentCount >= MAX_SEGMENTS)
                            return false;
                    }
                    lastSegmentSize = 0;
                }
                lastSegmentSize++;
                if (isSame && lastSegmentSize > 3)
                    return false;
            }

            //Segment count must always be a multiple of 2
            if (segmentCount > 0) {
                bool isEnd = segmentCount <= 2;
                Ptr<FastPoint<VAL>> point = factory->createFastPoint(lastId++, helper.spotlightValue,
                                                                     helper.getMinDistanceValue(), curRow, curCol, 1,
                                                                     isEnd);
                points.push_back(point);
#ifdef UIC_FPOINTDETECTOR_DEBUG
                debug.at<Vec3b>(curRow, curCol) = isEnd ? Vec3b(0, 255, 0) : Vec3b(255, 0, 0);
#endif
                return true;
            }
            return false;
        }


        template<class VAL>
        void FastPointHelper<VAL>::moveTo(const VAL *spotlight) {
            this->spotlight = spotlight;
            spotlightValue = spotlight[0];
            hasOtherValue = false;
        }

        /**
         * Loads and caches all values around the circle in isSame[] and checks if:
         *      - they are all either brighter or darker (Other values can only be brighter or darker. Not both)
         *      - The circle is not a straight line
         * @return if all checks passed
         */
        template<class VAL>
        bool FastPointHelper<VAL>::loadAndCheckValues() {
            double totalDistance = 0;
            if (this->adaptiveThreshold) {
                for (int i = 0; i < neighbourVisitor.size; i++) {
                    const VAL *neighbour = spotlight + neighbourVisitor.indices[i];
                    const VAL &neighbourValue = neighbour[0];
                    Scalar diff = factory->subtract(spotlightValue, neighbourValue);
                    totalDistance += norm(diff);
                }
                curThreshold = totalDistance / neighbourVisitor.size * adaptiveValue;
                curThreshold = MAX(curThreshold, threshold);
            } else {
                curThreshold = threshold;
            }
            int half = neighbourVisitor.size / 2;
            for (int i = 0; i < half; i++) {
                int opposite = (i + half) % neighbourVisitor.size;
                if (!loadValue(i) || !loadValue(opposite))
                    return false;
                if (isSame[i] && isSame[opposite])
                    return false;
            }
            return true;
        }

        /**
         * Loads the value on the circle into isSame[index]
         * @param index
         * @return if a valid value was loaded (No brighter and darker values were loaded before)
         */
        template<class VAL>
        bool FastPointHelper<VAL>::loadValue(int index) {
            const VAL *neighbour = spotlight + neighbourVisitor.indices[index];
            const VAL &neighbourValue = neighbour[0];
            Scalar diff = factory->subtract(spotlightValue, neighbourValue);
            double distance = norm(diff);
            this->isSame[index] = distance < curThreshold;
            if (!this->isSame[index]) {
                if (hasOtherValue) {
                    if (!factory->hasSameSign(otherDiff, diff))
                        return false;
                    if (distance < minDistance) {
                        minDistance = distance;
                        minDistanceValue = neighbourValue;
                    }
                } else {
                    hasOtherValue = true;
                    otherDiff = diff;
                    minDistance = distance;
                    minDistanceValue = neighbourValue;
                }
            }
//            0.00, 34.09, 76.23, 46.42, 61.12, 59.14, 156, 515

            return true;
        }

        template<class VAL>
        bool FastPointHelper<VAL>::isConnectedToCenter(int index) {
            const VAL *innerCircle = spotlight + connectivityVisitor.indices[index];
            return isSameAsCenter(*innerCircle);
        }

        template<class VAL>
        bool FastPointHelper<VAL>::isSameAsCenter(const VAL &value) {
            return factory->distance(value, spotlightValue) < curThreshold;
        }
    }
}


#endif //UIC_FPOINTDETECTOR_H
