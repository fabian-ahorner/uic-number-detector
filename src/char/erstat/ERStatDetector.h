//
// Created by fab on 19.02.17.
//

#ifndef UIC_ERDETECTOR_H
#define UIC_ERDETECTOR_H


#include "../CharacterDetector.h"
#include "ERStatCandidate.h"

namespace cv {
    namespace text {
        struct ErStatOptions : public CharDetectorOptions {
            string classifierNM1Path = "/home/fab/dev/cpp/uic/models/trained_classifierNM1.xml";
            string classifierNM2Path = "/home/fab/dev/cpp/uic/models/trained_classifierNM2.xml";
            int minThreshold = 4;
            float minProbabilityNM1 = 0.2;
            float minProbabilityThresholdNM1 = 0.1;
            float minProbabilityNM2 = 0.1f;
            bool nonMaxSuppression = false;
            int channels = ImageUtils::CHANNELS_ALL;
            inline String toString(){
                std::ostringstream str;
                str << CharDetectorOptions::toString();
                str << "minThreshold="<<minThreshold<<endl;
                str << "minProbabilityNM1="<<minProbabilityNM1<<endl;
                str << "minProbabilityThresholdNM1="<<minProbabilityThresholdNM1<<endl;
                str << "minProbabilityNM2="<<minProbabilityNM2<<endl;
                str << "nonMaxSuppression="<<nonMaxSuppression<<endl;
                str << "channels="<<channels<<endl;
                return str.str();
            }

            static ErStatOptions getRecall(){
                ErStatOptions options;
                options.minThreshold=1;
                options.minProbabilityNM1=0.1;
                options.minProbabilityNM2=0.9;
                return options;
            }
            static ErStatOptions getBalance(){
                ErStatOptions options;
                options.minThreshold=4;
                options.minProbabilityNM1=0.9;
                options.minProbabilityNM2=0.9;
                return options;
            }
            static ErStatOptions getSpeed(){
                ErStatOptions options;
                options.minThreshold=16;
                options.minProbabilityNM1=0.9;
                options.minProbabilityNM2=0.9;
                return options;
            }
        };
//        struct ErStatOptions : public CharDetectorOptions {
//            string classifierNM1Path = "/home/fab/dev/cpp/uic/models/trained_classifierNM1.xml";
//            string classifierNM2Path = "/home/fab/dev/cpp/uic/models/trained_classifierNM2.xml";
//            int minThreshold = 16;
//            float minProbabilityNM1 = 0.2;
//            float minProbabilityThresholdNM1 = 0.1;
//            float minProbabilityNM2 = 0.5;
//            bool nonMaxSuppression = true;
//        };

        class ErStatDetector : public CharacterDetector {
        private:
            Ptr<ERFilter> erFilterNM1, erFilterNM2;
        public:
            inline ErStatDetector(ErStatOptions options)
                    : CharacterDetector(options) {
                erFilterNM1 = createERFilterNM1(
                        loadClassifierNM1(options.classifierNM1Path),
                        options.minThreshold, 0.0000000001f, 1, options.minProbabilityNM1, options.nonMaxSuppression,
                        options.minProbabilityThresholdNM1);
                erFilterNM2 = createERFilterNM2(
                        loadClassifierNM2(options.classifierNM2Path),
                        options.minProbabilityNM2);
            }


            inline void findErStats(Mat img, vector<ERStat> &regions) {
                float area = img.rows * img.cols;
                float minArea = CharacterCandidate::MIN_CHAR_SIZE.area() / area;
                float maxArea = 1.f / (charsX * charsY);
                if (minArea < maxArea) {
                    erFilterNM1->setMinArea(minArea);
                    erFilterNM1->setMaxArea(maxArea);
                    erFilterNM1->run(img, regions);
                    erFilterNM2->run(img, regions);
                }
            }

            inline PCharVector processImage(Mat original, Mat img) {
                TimeLogger::logStart(getProcessName());
                vector<ERStat> regions;
                findErStats(img, regions);
                const PCharVector &result = ERCharacter::from(original, regions, new Mat(img),
                                                              getNextIdAndSkip((int) regions.size()));
                TimeLogger::logEnd(getProcessName());
                return result;
            }

            inline string getProcessName() {
                return "ErStatDetector";
            }
        };
    }
}


#endif //UIC_ERDETECTOR_H
