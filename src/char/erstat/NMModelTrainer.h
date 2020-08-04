//
// Created by fab on 19.10.16.
//

#ifndef UIC_MODELTRAINER_H
#define UIC_MODELTRAINER_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include <iostream>

using namespace cv;
using namespace std;

class ModelTrainer {
public:
    static void createModelNM1(const String &datasetPath, const String &modelPath);

    static void createModelNM2(const String &datasetPath, const String &modelPath);

    static void createDataNM1(const String &images, const String &outPath) ;

    static void createDataNM2(const String &images, const String &outPath);

private:
    static void extractDataNM1(const Mat& image, ostream &output, char type);
    static void extractDataNM2(const Mat& image, ostream &output, char type);

    static void exportImages(const vector<string> &imageFiles,
                             void (*extractDataFromImage)(const Mat &, ostream &, char),
                             ofstream &out, char type);

    static void createData(const String &images, const String &path, void (*nm2)(const Mat &, ostream &, char));
};


#endif //UIC_MODELTRAINER_H
