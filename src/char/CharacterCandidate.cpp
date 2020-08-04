//
// Created by fab on 10.11.16.
//

#include "CharacterCandidate.h"
#include "../cv/ImageUtils.h"
#include "fastext/NeighbourVisitor.h"
#include <opencv2/imgproc.hpp>


namespace cv {
    namespace text {
        const Size CharacterCandidate::MIN_CHAR_SIZE = Size(3, 5);

        CharacterCandidate::CharacterCandidate(Rect rect, int id)
                : rect(rect), center(getCenter(rect)),
//          radius(cv::getRadius(rect)),
//          radius(cv::getRadius(rect)),
//          width(cv::getRadius(rect) * 2),
//          height(cv::getRadius(rect) * 2),
                  radius(rect.height / 2),
                  width(rect.width),
                  height(rect.height),
                  id(id) {

        }

        int CharacterCandidate::getLineDistanceX() const {
            return center.x;
        }

        int CharacterCandidate::getLineDistanceY() const {
            return center.y;
        }

        int CharacterCandidate::getLeft() const {
            return rect.x;
        }

        int CharacterCandidate::getTop() const {
            return rect.y;
        }

        int CharacterCandidate::getRight() const {
            return rect.x + width ;
        }

        int CharacterCandidate::getBottom() const {
            return rect.y + height ;
        }

        void CharacterCandidate::setLineDistance(const Point &lineDistance) {
//            this->transformedPos = lineDistance;
        }

        Rect CharacterCandidate::getRect() const {
            return rect;
        }

        Scalar CharacterCandidate::getMean() const {
            return mean;
        }

        Scalar CharacterCandidate::getStandardDeviation() const {
            return standardDeviation;
        }

        double CharacterCandidate::getIsCharProbability() {
            return 1;
        }

        double CharacterCandidate::getCharQuality() {
            return 1;
        }

        bool CharacterCandidate::isCompatible(const Ptr<CharacterCandidate> &other) {
            return true;
        }

        void CharacterCandidate::setPrediction(char prediction, double confidence) {
            this->prediction=prediction;
            this->confidence=confidence;
        }
    }
}
