//
// Created by fab on 22.10.16.
//

#include "CharClassifier.h"
#include "../cv/ImageUtils.h"
#include "../utils/IOUtils.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

Mat text::CharClassifier::standardise(Mat img) {
    vector<vector<Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    Mat contourImage;
    img.copyTo(contourImage);
    findContours(contourImage, contours, hierarchy, CV_RETR_CCOMP,
                 CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

    Rect boundingBox;
    for (int i = 0; i < contours.size(); i++) {// iterate through each contour.
        boundingBox = merge(boundingBox,
                            boundingRect(contours[i])); // Find the bounding rectangle for biggest contour
    }
    if (boundingBox.height <= 2 || boundingBox.width <= 2)
        return Mat();
    img(boundingBox).copyTo(img);
    img = resizeWithBorder(img, CHAR_CLASSIFIER_SIZE, CHAR_CLASSIFIER_SIZE);
    img.convertTo(img, CV_32F);
    return img;
}

void text::CharClassifier::createDataset(string path, Mat &features, Mat &classes) {
    const vector<string> &files = cmp::IOUtils::GetFilesInDirectory(path, "*.png", true);

    vector<string>::const_iterator file;
    int current = 0;
    const Mat &rot1 = getRotationMatrix2D(Point(CHAR_CLASSIFIER_SIZE / 2, CHAR_CLASSIFIER_SIZE / 2), 20, 1);
    const Mat &rot2 = getRotationMatrix2D(Point(CHAR_CLASSIFIER_SIZE / 2, CHAR_CLASSIFIER_SIZE / 2), -20, 1);
    for (file = files.begin(); file != files.end(); file++) {
        printf("%3.2f%% %s\n", current++ * 100. / files.size(), file->c_str());

        Mat img = imread(*file, CV_BGR2GRAY);
        img = standardise(img);
        int character = getCharacterNumber(path, *file);
        Mat element = getStructuringElement( MORPH_RECT,
                                             Size(  2, 2 ),
                                             Point( 1, 1 ) );
        if (img.cols > 0 && img.rows > 0 && character<10) {
            Mat imgTransformed;
//            cout << "Class:" << character << endl;
//            imshow("debug", img);
//            waitKey(-1);

            // Add original version
            addToDataset(img, character, features, classes);


            /// Apply the erosion operation
            dilate( img, imgTransformed, element );
            addToDataset(imgTransformed, character, features, classes);
//            namedWindow("Erosion Demo", CV_WINDOW_NORMAL);
//            imshow( "Erosion Demo", imgTransformed );
//            waitKey();

            // Add blurred version
//            GaussianBlur(img, imgTransformed, Size(3, 3), 0, 0);
//            addToDataset(img, character, features, classes);

            // Add rotated version 1
//            warpAffine(img, imgTransformed, rot1, Size(img.cols, img.rows));
//            addToDataset(imgTransformed, character, features, classes);

            // Add rotated and blurred version 1
//            GaussianBlur(imgTransformed, imgTransformed, Size(3, 3), 0, 0);
//            addToDataset(img, character, features, classes);

            // Add rotated version 2
//            warpAffine(img, imgTransformed, rot2, Size(img.cols, img.rows));
//            addToDataset(imgTransformed, character, features, classes);

            // Add rotated and blurred version 2
//            GaussianBlur(imgTransformed, imgTransformed, Size(3, 3), 0, 0);
//            addToDataset(img, character, features, classes);
        }
    }

//    imshow("debug", features);
//    waitKey(-1);
}

int text::CharClassifier::getCharacterNumber(const string &path,
                                             const string &file) {
    string fileName;
    char ch = file[path.size()];
    if (ch == '/') {
        fileName = file.substr(path.size() + 1, file.size());
    } else {
        fileName = file.substr(path.size(), file.size());
    }
    istringstream iFileName(fileName);
    int character;
    iFileName >> character;
    character -= '0';
    if (character < 0 || character > 9)
        character = 10;
//    character = character == 2 ? 1 : 0;
    return character;
}

void text::CharClassifier::createDataset(string path, string featuresFile, string classesFile) {
    Mat features, classes;
    CharClassifier::createDataset(path,
                                  features, classes);
    features.convertTo(features, CV_8UC3, 255.0);
    classes.convertTo(classes, CV_8UC3, 255.0);
    imwrite(featuresFile, features);
    imwrite(classesFile, classes);
}

Ptr<ml::TrainData> text::CharClassifier::loadDataset(string featuresFile, string classesFile) {
    Mat features, classes;
    features = imread(featuresFile, CV_BGR2GRAY);
    classes = imread(classesFile, CV_BGR2GRAY);
    features.convertTo(features, CV_32F);
    classes.convertTo(classes, CV_32S);
    return ml::TrainData::create(features, ml::ROW_SAMPLE, classes);
}

Ptr<ml::TrainData> text::CharClassifier::createDataset(string path) {
    Mat features, classes;
    CharClassifier::createDataset(path,
                                  features, classes);
    return ml::TrainData::create(features, ml::ROW_SAMPLE, classes);
}

float text::CharClassifier::calcError(Ptr<ml::StatModel> ml, Ptr<ml::TrainData> data, bool test) {
    Mat response;
    float fl1 = ml->calcError(data, test, response);

    Mat originals = test ? data->getTestResponses() : data->getTrainResponses();

//    cout << "True: " << originals.type() << endl << originals << endl;
//    cout << "Result: " << response.type() << endl << response << endl;

    MatIterator_<int> itOriginals = originals.begin<int>();
    MatIterator_<float> itResults = response.begin<float>();
    const Mat &lables = data->getClassLabels();
    int classN = data->getClassLabels().rows;
    Mat confusion = Mat::zeros(classN, classN, CV_32S);
    float errors = 0;
    for (; itOriginals < originals.end<int>(); itOriginals++, itResults++) {
        int r = (int) (*itResults + 0.5);
        int o = *itOriginals;
        if (r != o)
            errors++;
        confusion.at<int>(r, o)++;
    }
    cout << confusion << endl;
    return errors * 100 / originals.rows;
}

/**
 * CharClassifier::createClassifier("/home/fab/dev/java/CharacterFactory/numbers","/home/fab/dev/cpp/uic/character_classifier.xml")
 * @param folder
 * @param classifierFile
 * @return
 */
Ptr<ml::StatModel> text::CharClassifier::createClassifier(string folder, string classifierFile) {
    Ptr<TrainData> data = CharClassifier::createDataset(folder);
//    data->setTrainTestSplitRatio(0.9999, true);
    data->shuffleTrainTest();
    Ptr<SVM> ml = SVM::create();
    ml->setKernel(SVM::INTER);
//    ml->setBoostType(Boost::DISCRETE);
//    ml->setWeakCount(100);
//    ml->setWeightTrimRate(0.5);
//    ml->setP(3);
//    Ptr<KNearest> ml = KNearest::create();
//    ml->setIsClassifier(true);
//    ml->setDefaultK(3);

    ml->trainAuto(data);
    ml->save(classifierFile);

    float fl1 = CharClassifier::calcError(ml, data, false);
//    float fl2 = CharClassifier::calcError(ml, data, true);
    printf("Error train %f \n", fl1);
//    printf("Error test %f \n", fl2);
    return ml;
}

void text::CharClassifier::addToDataset(Mat img, int character, Mat &features, Mat &classes) {
    img = img.reshape(1, 1);
    img.convertTo(img, CV_32F);
    features.push_back(img);
    classes.push_back(character);
}
