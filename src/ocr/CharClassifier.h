//
// Created by fab on 22.10.16.
//

#ifndef UIC_CHARCLASSIFIER_H
#define UIC_CHARCLASSIFIER_H

#include "opencv2/core/core.hpp"
#include "string"
#include <opencv2/ml.hpp>

using namespace std;
using namespace cv::ml;
namespace cv {
    namespace text {
        class CharClassifier {
        public:
            static const int CHAR_CLASSIFIER_SIZE = 16;

            static Mat standardise(Mat img);

            static void createDataset(string path, string featuresFile, string classesFile);

            static Ptr<ml::TrainData> createDataset(string path);

            static Ptr<ml::TrainData> loadDataset(string featuresFile, string classesFile);

            static void createDataset(string path, Mat &feature, Mat &classes);

            static int getCharacterNumber(const string &path,
                                          const string &file);

            static float calcError(Ptr<ml::StatModel> ml, Ptr<ml::TrainData> data, bool train);

            static Ptr<ml::StatModel> createClassifier(string folder, string classifierFile);

        private:
            static void addToDataset(Mat mat, int character, Mat &features, Mat &classes);
        };
    }
}


#endif //UIC_CHARCLASSIFIER_H
