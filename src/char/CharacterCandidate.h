//
// Created by fab on 10.11.16.
//

#ifndef UIC_CHARCANDIDATE_H
#define UIC_CHARCANDIDATE_H

#include <opencv2/core/core.hpp>
#include <opencv2/text.hpp>
#include <vector>
#include <opencv2/imgproc.hpp>

namespace cv {
    namespace text {
        using namespace std;

        class CharacterCandidate {
//            Point transformedPos;
        protected:
            Scalar mean, standardDeviation;
        public:
            static const Size MIN_CHAR_SIZE;
            static const int MIN_CHAR_AREA = 5;

            const int radius;
            const int width;
            const int height;
            const int id;
            const Rect rect;
            const Point center;
            char prediction = 0;
            double confidence = 0;

            CharacterCandidate(Rect rect, int id);

            int getLineDistanceX() const;

            int getLeft() const;

            int getRight() const;

            int getLineDistanceY() const;

            int getTop() const;

            int getBottom() const;

            Scalar getMean() const;

            Scalar getStandardDeviation() const;

            void setLineDistance(const Point &lineDistance);

            virtual inline char getPrediction() const {
                return prediction;
            }

            virtual inline void setPrediction(char prediction) {
                this->prediction = prediction;
            }

            bool operator<(const CharacterCandidate &other) const {
                if (center.x == other.center.x)
                    return id < other.id;
                return center.x < other.center.x;
            }

            static bool sortByTransformedX(const Ptr<CharacterCandidate> &c1, const Ptr<CharacterCandidate> &c2) {
                return (*c1) < (*c2);
            }


            virtual bool isCompatible(const Ptr<CharacterCandidate> &other);

            virtual Mat getImage()=0;

            virtual void draw(Mat img)=0;

            virtual double getIsCharProbability();

            virtual double getCharQuality();

            Rect getRect() const;

            virtual void setPrediction(char prediction, double confidence);

            virtual double getConfidence() {
                return confidence;
            }
        };

        typedef Ptr<CharacterCandidate> PCharacterCandidate;
        typedef vector<PCharacterCandidate> CharVector;
        typedef Ptr<CharVector> PCharVector;

        struct SortCharByTransformedX {
            inline bool operator()(const Ptr<CharacterCandidate> &c1, const Ptr<CharacterCandidate> &c2) {
                return (*c1) < (*c2);
            }
        };

        struct SortCharByTransformedLeft {
            inline bool operator()(const Ptr<CharacterCandidate> &c1, const Ptr<CharacterCandidate> &c2) {
                int diff = c1->getLeft() - c2->getLeft();
                if (diff != 0)
                    return diff < 0;
                return c1->id < c2->id;
//                return c1->getLeft() < c2->getLeft();
            }
        };

        struct SortCharByTransformedRight {
            inline bool operator()(const Ptr<CharacterCandidate> &c1, const Ptr<CharacterCandidate> &c2) {
                return c1->getRight() < c2->getRight();
            }
        };
    }
}
#endif //UIC_CHARCANDIDATE_H
