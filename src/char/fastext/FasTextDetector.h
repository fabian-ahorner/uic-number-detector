//
// Created by fab on 24.11.16.
//

#ifndef UIC_FASTDETECTOR_H
#define UIC_FASTDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <vector>
#include <queue>
#include <iostream>
#include "../../utils/TimeLogger.h"
#include "../CharacterDetector.h"
#include "FastPointDetector.h"
#include "FastPointSegmenter.h"
#include "FastCharacter.h"
#include "FasTextOptions.h"
#include "../ScaledCharacter.h"

using namespace std;
namespace cv {
    namespace text {
        using namespace ml;

        template<class VAL>
        class FasTextDetector : public CharacterDetector {
        private:
            FasTextOptions options;
            Ptr<StatModel> classifierNM1;
            Ptr<StatModel> classifierNM2;
            Size minSize;
        public:
            FasTextDetector(FasTextOptions options);
//            void detect(const Mat &img);

            virtual PCharVector processImage(Mat original, Mat img);

            PCharVector extractCandidates(Mat img);

            virtual Ptr<FastPointFactory<VAL>> getFactory()=0;

            inline string getName() {
                return "FasTextDetector";
            }
        };


        template<class VAL>
        FasTextDetector<VAL>::FasTextDetector(FasTextOptions options)
                : CharacterDetector(options) {
            this->options = options;
            classifierNM1 = StatModel::load<Boost>(options.classifierNM1Path);
            classifierNM2 = StatModel::load<Boost>(options.classifierNM2Path);
            this->minSize = getMinSize(options.charsX, options.charsY);
        }

        template<class VAL>
        PCharVector FasTextDetector<VAL>::processImage(Mat original, Mat img) {
            TimeLogger::logStart(toString());
            const PCharVector &result = extractCandidates(img);
            TimeLogger::logEnd(toString());
            return result;
        }

        template<class VAL>
        PCharVector FasTextDetector<VAL>::extractCandidates(Mat img) {

            Ptr<FastPointFactory<VAL>> factory = getFactory();
            FastPointDetector<VAL> pointDetector(factory, img, options.minThreshold, options.nonMaxSuppression, options.adaptiveThreshold, options.adaptiveValue);
            vector<Ptr<FastPoint<VAL>>> points = pointDetector.detectKeypoints();

            FastPointSegmenter<VAL> pointSegmenter(factory, img, options);
            pointSegmenter.setClassifierNM1(classifierNM1);
            pointSegmenter.setClassifierNM2(classifierNM2);
            const vector<Ptr<FastCandidate<VAL>>> candidates = pointSegmenter.extractCandidates(points);

            return FastCharacter<VAL>::from(candidates, pointSegmenter.getMask(), img,
                                            getNextIdAndSkip((int) candidates.size()));
        }

        class FasTextDetector1C : public FasTextDetector<uchar> {
        public:
            inline FasTextDetector1C(FasTextOptions options) : FasTextDetector(options) {}

            inline string getProcessName() {
                return "FasTextDetector1C";
            }

            Ptr<FastPointFactory<uchar>> getFactory() {
                return new FastPointFactory1C();
            }

//            PCharVector processImage(Mat img)

        };

        class FasTextDetector3C : public FasTextDetector<Vec3b> {
        public:
            inline FasTextDetector3C(FasTextOptions options) : FasTextDetector(options) {}

            inline string getProcessName() {
                return "FasTextDetector3C";
            }

            Ptr<FastPointFactory<Vec3b>> getFactory() {
                return new FastPointFactory3C();
            }

            inline PCharVector processImage(Mat original, Mat img) {
                // Use original image in LAB color channel
                return FasTextDetector<Vec3b>::processImage(img, img);
            }
        };

    }
}


#endif //UIC_FASTDETECTOR_H
