//
// Created by fab on 19.10.16.
//

#include "NMModelTrainer.h"
#include "../../utils/IOUtils.h"
#include <fstream>
#include "opencv2/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"

#define PI 3.14159265

using namespace cv::ml;

using namespace std;
using namespace cv;
/**
 * //    ModelTrainer::createDataNM1(DATASET_NOCHAR_CHAR,
//                                "/home/fab/dev/cpp/uic/char_datasetNM1_number.csv");
//    ModelTrainer::createModelNM1("/home/fab/dev/cpp/uic/char_datasetNM1_number.csv",
//                                 "/home/fab/dev/cpp/uic/trained_classifierNM1_number.xml");
//    ModelTrainer::createDataNM2(DATASET_NOCHAR_CHAR,
//                                "/home/fab/dev/cpp/uic/char_datasetNM2_number.csv");
//    ModelTrainer::createModelNM2("/home/fab/dev/cpp/uic/char_datasetNM2_number.csv",
//                                 "/home/fab/dev/cpp/uic/trained_classifierNM2_number.xml");
 */

/**
 * @param dataset "/home/fab/datasets/erfilter_train/char_datasetNM1.csv"
 * @param model "/home/fab/dev/cpp/uic/trained_classifierNM1.xml"
 */
void ModelTrainer::createModelNM1(const String &datasetPath, const String &modelPath) {
    Ptr<TrainData> cvml = TrainData::loadFromCSV(datasetPath, 0,
                                                 0);

    cvml->setTrainTestSplitRatio(0.9, true);

    Ptr<Boost> boost;

    ifstream ifile(modelPath);
    if (ifile) {
        //The file exists, so we don't want to train
        printf("Found trained_boost_char.xml file, remove it if you want to retrain with new data ... \n");
        boost = StatModel::load<Boost>(modelPath);
    } else {
        //Train with 100 features
        printf("Training ... \n");
        boost = Boost::create();
        boost->setBoostType(Boost::REAL);
        boost->setWeakCount(100);
        boost->setWeightTrimRate(0);
        boost->setMaxDepth(1);
        boost->setUseSurrogates(false);
        boost->setPriors(Mat());
        boost->train(cvml);
//        boost = StatModel::train<Boost>(cvml, Boost::Params(Boost::REAL, 100, 0.0, 1, false, Mat()));
    }

//Calculate the test and train errors
    Mat train_responses, test_responses;
    float fl1 = boost->calcError(cvml, false, train_responses);
    float fl2 = boost->calcError(cvml, true, test_responses);
    printf("Error train %f \n", fl1);
    printf("Error test %f \n", fl2);


//Try a char
//    Mat sample = (Mat_<float>(1, 4) << 1.063830f, 0.083372f, 0.000000f, 2.000000f);
    Mat sample = (Mat_<float>(1, 4) << 0.960000, 0.098915, 1.000000, 4.000000);
    float prediction = boost->predict(sample, noArray(), 0);
    float votes = boost->predict(sample, noArray(), DTrees::PREDICT_SUM | StatModel::RAW_OUTPUT);

    printf("\n The char sample is predicted as: %f (with number of votes = %f)\n", prediction, votes);
    printf(" Class probability (using Logistic Correction) is P(r|character) = %f\n",
           (float) 1 - (float) 1 / (1 + exp(-2 * votes)));

//Try a NONchar
//    Mat sample2 = (Mat_<float>(1, 4) << 2.000000, 0.235702, 0.000000, 2.000000);
    Mat sample2 = (Mat_<float>(1, 4) << 1.120370, 0.032635, 107.000000, 8.000000);

    prediction = boost->predict(Mat(sample2), noArray(), 0);
    votes = boost->predict(Mat(sample2), noArray(), DTrees::PREDICT_SUM | StatModel::RAW_OUTPUT);

    printf("\n The non_char sample is predicted as: %f (with number of votes = %f)\n", prediction, votes);
    printf(" Class probability (using Logistic Correction) is P(r|character) = %f\n\n",
           (float) 1 - (float) 1 / (1 + exp(-2 * votes)));

// Save the trained classifierNM1
    boost->save(string(modelPath));
}

void ModelTrainer::extractDataNM1(const Mat &_originalImage, ostream &output, char type) {
    Mat originalImage(_originalImage.rows + 2, _originalImage.cols + 2, _originalImage.type());
    copyMakeBorder(_originalImage, originalImage, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(255, 255, 255));

    Mat bwImage(originalImage.size(), CV_8UC1);

    uchar thresholdValue = 100;
    uchar maxValue = 255;
    uchar middleValue = 192;
    uchar zeroValue = 0;
    Scalar middleScalar(middleValue);
    Scalar zeroScalar(zeroValue);

    static int neigborsCount = 4;
    static int dx[] = {-1, 0, 0, 1};
    static int dy[] = {0, -1, 1, 0};
    int di, rx, ry;
    int perimeter;

    threshold(originalImage, bwImage, thresholdValue, maxValue, THRESH_BINARY_INV);

    int regionsCount = 0;
    int totalPixelCount = bwImage.rows * bwImage.cols;
    Point seedPoint;
    Rect rectFilled;
    int valuesSum, q1, q2, q3;
    bool p00, p10, p01, p11;

    for (int i = 0; i < totalPixelCount; i++) {
        if (bwImage.data[i] == maxValue) {
            seedPoint.x = i % bwImage.cols;
            seedPoint.y = i / bwImage.cols;

            if ((seedPoint.x == 0) || (seedPoint.y == 0) || (seedPoint.x == bwImage.cols - 1) ||
                (seedPoint.y == bwImage.rows - 1)) {
                continue;
            }

            regionsCount++;

            size_t pixelsFilled = floodFill(bwImage, seedPoint, middleScalar, &rectFilled);

            perimeter = 0;
            q1 = 0;
            q2 = 0;
            q3 = 0;

            int crossings[rectFilled.height];
            for (int j = 0; j < rectFilled.height; j++) {
                crossings[j] = 0;
            }

            for (ry = rectFilled.y - 1; ry <= rectFilled.y + rectFilled.height; ry++) {
                for (rx = rectFilled.x - 1; rx <= rectFilled.x + rectFilled.width; rx++) {
                    if ((bwImage.at<uint8_t>(ry, rx - 1) != bwImage.at<uint8_t>(ry, rx)) &&
                        (bwImage.at<uint8_t>(ry, rx - 1) + bwImage.at<uint8_t>(ry, rx) == middleValue + zeroValue)) {
                        crossings[ry - rectFilled.y]++;
                    }

                    if (bwImage.at<uint8_t>(ry, rx) == middleValue) {
                        for (di = 0; di < neigborsCount; di++) {
                            int xNew = rx + dx[di];
                            int yNew = ry + dy[di];

                            if (bwImage.at<uint8_t>(yNew, xNew) == zeroValue) {
                                perimeter++;
                            }
                        }
                    }

                    p00 = bwImage.at<uint8_t>(ry, rx) == middleValue;
                    p01 = bwImage.at<uint8_t>(ry, rx + 1) == middleValue;
                    p10 = bwImage.at<uint8_t>(ry + 1, rx) == middleValue;
                    p11 = bwImage.at<uint8_t>(ry + 1, rx + 1) == middleValue;
                    valuesSum = p00 + p01 + p10 + p11;

                    if (valuesSum == 1) q1++;
                    else if (valuesSum == 3) q2++;
                    else if ((valuesSum == 2) && (p00 == p11)) q3++;
                }
            }

            q1 = q1 - q2 + 2 * q3;
            if (q1 % 4 != 0) {
                printf("Non-integer Euler number");
                exit(0);
            }
            q1 /= 4;

            /*printf("New region: %d\n", regionsCount);
            printf("Area: %d\n", (int)pixelsFilled);
            printf("Bounding box (%d; %d) + (%d; %d)\n", rectFilled.x - 1, rectFilled.y - 1, rectFilled.width, rectFilled.height);
            printf("Perimeter: %d\n", (int)perimeter);
            printf("Euler number: %d\n", q1);
            printf("Crossings: ");
            for(int j = 0; j < rectFilled.height; j++)
            {
                printf("%d ", crossings[j]);
            }*/

            vector<int> m_crossings;
            m_crossings.push_back(crossings[(int) rectFilled.height / 6]);
            m_crossings.push_back(crossings[(int) 3 * rectFilled.height / 6]);
            m_crossings.push_back(crossings[(int) 5 * rectFilled.height / 6]);
            sort(m_crossings.begin(), m_crossings.end());

            //Features used in the first stage classifierNM1
            //aspect ratio (w/h), compactness (sqrt(a/p), number of holes (1 − η), and a horizontal crossings feature (cˆ = median {c_1*w/6, c_3*w/6, c_5*w/6}) which estimates number of character strokes in horizontal projection
            if ((rectFilled.width >= 20) &&
                (rectFilled.height >= 20)) { // TODO find a better way to select good negative examples
//                printf("%f,%f,%f,%f\n", (float) rectFilled.width / rectFilled.height, sqrt(pixelsFilled) / perimeter,
//                       (float) (1 - q1), (float) m_crossings.at(1));
                float aspect = (float) rectFilled.width / rectFilled.height;
                float compactness = (float) sqrt(pixelsFilled) / perimeter;
                float holes = (float) (1 - q1);
                float cross = (float) m_crossings.at(1);
                output << type << "," << aspect << "," << compactness << "," << holes << "," << cross << endl;
            }
            floodFill(bwImage, seedPoint, zeroScalar);
        }
    }

}

void ModelTrainer::createDataNM1(const String &images, const String &outPath) {
    createData(images, outPath, extractDataNM1);
}

void ModelTrainer::createData(const String &images, const String &outPath,
                              void (*extractData)(const Mat &, ostream &, char)) {
    string charDir = cmp::IOUtils::CombinePath(images, "char");
    string noCharDir = cmp::IOUtils::CombinePath(images, "nochar");
    const vector<string> &charFiles = cmp::IOUtils::GetFilesInDirectory(charDir, "*.tiff", true);
    const vector<string> &noCharFiles = cmp::IOUtils::GetFilesInDirectory(noCharDir, "*.tiff", true);
    ofstream out(outPath);
    exportImages(charFiles, extractData, out, 'C');
    exportImages(noCharFiles, extractData, out, 'N');
    out.close();
}

void ModelTrainer::exportImages(const vector<string> &imageFiles,
                                void (*extractDataFromImage)(const Mat &, ostream &, char),
                                ofstream &out, char type) {
    for (int i = 0; i < imageFiles.size(); ++i) {
        Mat originalImage = imread(imageFiles[i], 0);
        originalImage = 255 - originalImage;
        extractDataFromImage(originalImage, out, type);
    }
}

void ModelTrainer::extractDataNM2(const Mat &_originalImage, ostream &output, char type) {
    Mat originalImage(_originalImage.rows + 2, _originalImage.cols + 2, _originalImage.type());
    copyMakeBorder(_originalImage, originalImage, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(255, 255, 255));

    Mat bwImage(originalImage.size(), CV_8UC1);

    uchar thresholdValue = 100;
    uchar maxValue = 255;
    uchar middleValue = 192;
    uchar zeroValue = 0;
    Scalar middleScalar(middleValue);
    Scalar zeroScalar(zeroValue);

    static int neigborsCount = 4;
    static int dx[] = {-1, 0, 0, 1};
    static int dy[] = {0, -1, 1, 0};
    int di, rx, ry;
    int perimeter;

    //cvtColor(originalImage, bwImage, CV_RGB2GRAY);
    threshold(originalImage, bwImage, thresholdValue, maxValue, THRESH_BINARY_INV);

    int regionsCount = 0;
    int totalPixelCount = bwImage.rows * bwImage.cols;
    Point seedPoint;
    Rect rectFilled;
    int valuesSum, q1, q2, q3;
    bool p00, p10, p01, p11;

    for (int i = 0; i < totalPixelCount; i++) {
        if (bwImage.data[i] == maxValue) {
            seedPoint.x = i % bwImage.cols;
            seedPoint.y = i / bwImage.cols;

            if ((seedPoint.x == 0) || (seedPoint.y == 0) || (seedPoint.x == bwImage.cols - 1) ||
                (seedPoint.y == bwImage.rows - 1)) {
                continue;
            }

            regionsCount++;

            size_t pixelsFilled = floodFill(bwImage, seedPoint, middleScalar, &rectFilled);

            perimeter = 0;
            q1 = 0;
            q2 = 0;
            q3 = 0;

            int crossings[rectFilled.height];
            for (int j = 0; j < rectFilled.height; j++) {
                crossings[j] = 0;
            }

            for (ry = rectFilled.y - 1; ry <= rectFilled.y + rectFilled.height; ry++) {
                for (rx = rectFilled.x - 1; rx <= rectFilled.x + rectFilled.width; rx++) {
                    if ((bwImage.at<uint8_t>(ry, rx - 1) != bwImage.at<uint8_t>(ry, rx)) &&
                        (bwImage.at<uint8_t>(ry, rx - 1) + bwImage.at<uint8_t>(ry, rx) == middleValue + zeroValue)) {
                        crossings[ry - rectFilled.y]++;
                    }

                    if (bwImage.at<uint8_t>(ry, rx) == middleValue) {
                        for (di = 0; di < neigborsCount; di++) {
                            int xNew = rx + dx[di];
                            int yNew = ry + dy[di];

                            if (bwImage.at<uint8_t>(yNew, xNew) == zeroValue) {
                                perimeter++;
                            }
                        }
                    }

                    p00 = bwImage.at<uint8_t>(ry, rx) == middleValue;
                    p01 = bwImage.at<uint8_t>(ry, rx + 1) == middleValue;
                    p10 = bwImage.at<uint8_t>(ry + 1, rx) == middleValue;
                    p11 = bwImage.at<uint8_t>(ry + 1, rx + 1) == middleValue;
                    valuesSum = p00 + p01 + p10 + p11;

                    if (valuesSum == 1) q1++;
                    else if (valuesSum == 3) q2++;
                    else if ((valuesSum == 2) && (p00 == p11)) q3++;
                }
            }

            q1 = q1 - q2 + 2 * q3;
            if (q1 % 4 != 0) {
                printf("Non-integer Euler number");
                exit(0);
            }
            q1 /= 4;

            /*printf("New region: %d\n", regionsCount);
            printf("Area: %d\n", (int)pixelsFilled);
            printf("Bounding box (%d; %d) + (%d; %d)\n", rectFilled.x - 1, rectFilled.y - 1, rectFilled.width, rectFilled.height);
            printf("Perimeter: %d\n", (int)perimeter);
            printf("Euler number: %d\n", q1);
            printf("Crossings: ");
            for(int j = 0; j < rectFilled.height; j++)
            {
                printf("%d ", crossings[j]);
            }*/

            vector<int> m_crossings;
            m_crossings.push_back(crossings[(int) rectFilled.height / 6]);
            m_crossings.push_back(crossings[(int) 3 * rectFilled.height / 6]);
            m_crossings.push_back(crossings[(int) 5 * rectFilled.height / 6]);
            sort(m_crossings.begin(), m_crossings.end());

            //Features used in the first stage classifierNM1
            //aspect ratio (w/h), compactness (sqrt(a/p), number of holes (1 − η), and a horizontal crossings feature (cˆ = median {c_1*w/6, c_3*w/6, c_5*w/6}) which estimates number of character strokes in horizontal projection
            if ((rectFilled.width >= 3) &&
                (rectFilled.height >= 3)) // TODO find a better way to select good negative examples
            {

                float aspect = (float) rectFilled.width / rectFilled.height;
                float compactness = (float) sqrt(pixelsFilled) / perimeter;
                float holes = (float) (1 - q1);
                float cross = (float) m_crossings.at(1);
                output << type << "," << aspect << "," << compactness << "," << holes << "," << cross;
//                printf("%f,%f,%f,%f,", (float) rectFilled.width / rectFilled.height, sqrt(pixelsFilled) / perimeter,
//                       (float) (1 - q1), (float) m_crossings.at(1));

                Mat region = Mat::zeros(bwImage.rows + 2, bwImage.cols + 2, CV_8UC1);
                int newMaskVal = 255;
                int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE;
                Rect rect;
                floodFill(bwImage, region, seedPoint, zeroScalar, &rect, Scalar(), Scalar(), flags);
                rect.width += 2;
                rect.height += 2;
                region = region(rect);

                vector<vector<Point> > contours;
                vector<Point> contour_poly;
                vector<Vec4i> hierarchy;
                findContours(region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));
                //TODO check epsilon parameter of approxPolyDP (set empirically) : we want more precission if the region is very small because otherwise we'll loose all the convexities
                approxPolyDP(Mat(contours[0]), contour_poly, (float) min(rect.width, rect.height) / 17, true);

                bool was_convex = false;
                int num_inflexion_points = 0;
                for (int p = 0; p < contour_poly.size(); p++) {
                    int p_prev = p - 1;
                    int p_next = p + 1;
                    if (p_prev == -1)
                        p_prev = contour_poly.size() - 1;
                    if (p_next == contour_poly.size())
                        p_next = 0;

                    double angle_next = atan2((contour_poly[p_next].y - contour_poly[p].y),
                                              (contour_poly[p_next].x - contour_poly[p].x));
                    double angle_prev = atan2((contour_poly[p_prev].y - contour_poly[p].y),
                                              (contour_poly[p_prev].x - contour_poly[p].x));
                    if (angle_next < 0)
                        angle_next = 2. * PI + angle_next;

                    double angle = (angle_next - angle_prev);
                    if (angle > 2. * PI)
                        angle = angle - 2. * PI;
                    else if (angle < 0)
                        angle = 2. * PI + abs(angle);

                    if (p > 0) {
                        if (((angle > PI) && (!was_convex)) || ((angle < PI) && (was_convex)))
                            num_inflexion_points++;
                    }
                    was_convex = (angle > PI);

                    //cout << "(" << contour_poly[p].x<<","<<contour_poly[p].y << ") angle "<<angle<<endl;

                }

                floodFill(region, Point(0, 0), Scalar(255), 0);
                int holes_area = region.cols * region.rows - countNonZero(region);

                int hull_area = 0;

                {

                    vector<Point> hull;
                    cv::convexHull(contours[0], hull, false);
                    hull_area = contourArea(hull);
                }

                output << "," << ((float) holes_area / pixelsFilled) << ","
                       << ((float) hull_area / contourArea(contours[0])) << "," << ((float) num_inflexion_points)
                       << endl;

//                printf("%f,%f,%f\n", (float) holes_area / pixelsFilled, (float) hull_area / contourArea(contours[0]),
//                       (float) num_inflexion_points);

            } else {
                floodFill(bwImage, seedPoint, zeroScalar);
            }

        }
    }
}

void ModelTrainer::createDataNM2(const String &images, const String &outPath) {
    createData(images, outPath, extractDataNM2);
}

/**
 * @param dataset "/home/fab/datasets/erfilter_train/char_datasetNM2.csv"
 * @param model "/home/fab/dev/cpp/uic/trained_classifierNM2.xml"
 */
void ModelTrainer::createModelNM2(const String &datasetPath, const String &modelPath) {
    Ptr<TrainData> cvml = TrainData::loadFromCSV(datasetPath, 0,
                                                 0);

//Select 90% for the training
    cvml->setTrainTestSplitRatio(0.8, true);

    Ptr<Boost> boost;

    ifstream ifile(modelPath);
    if (ifile) {
        //The file exists, so we don't want to train
        printf("Found trained_boost_char.xml file, remove it if you want to retrain with new data ... \n");
        boost = StatModel::load<Boost>(modelPath);
    } else {
        //Train with 100 features
        printf("Training ... \n");
        boost = Boost::create();
        boost->setBoostType(Boost::REAL);
        boost->setWeakCount(100);
        boost->setWeightTrimRate(0);
        boost->setMaxDepth(1);
        boost->setUseSurrogates(false);
        boost->setPriors(Mat());
        boost->train(cvml);
//        boost = StatModel::train<Boost>(cvml, Boost::Params(Boost::REAL, 100, 0.0, 1, false, Mat()));
    }

//Calculate the test and train errors
    Mat train_responses, test_responses;
    float fl1 = boost->calcError(cvml, false, train_responses);
    float fl2 = boost->calcError(cvml, true, test_responses);
    printf("Error train %f \n", fl1);
    printf("Error test %f \n", fl2);


//Try a char
//    Mat sample = (Mat_<float>(1, 7) << 0.870690, 0.096485, 2.000000, 2.000000, 0.137080, 1.269940, 2.000000);
    Mat sample = (Mat_<float>(1, 7) << 0.983146,0.134923,1,4,0.268738,1.00972,0);
    float prediction = boost->predict(sample, noArray(), 0);
    float votes = boost->predict(sample, noArray(), DTrees::PREDICT_SUM | StatModel::RAW_OUTPUT);

    printf("\n The char sample is predicted as: %f (with number of votes = %f)\n", prediction, votes);
    printf(" Class probability (using Logistic Correction) is P(r|character) = %f\n",
           (float) 1 - (float) 1 / (1 + exp(-2 * votes)));

//Try a NONchar
//    Mat sample2 = (Mat_<float>(1, 7) << 0.565217, 0.103749, 1.000000, 2.000000, 0.032258, 1.525692, 10.000000);
    Mat sample2 = (Mat_<float>(1, 7) << 0.6875,0.0657129,1,4,0.0717703,2.84177,18);

    prediction = boost->predict(Mat(sample2), noArray(), 0);
    votes = boost->predict(Mat(sample2), noArray(), DTrees::PREDICT_SUM | StatModel::RAW_OUTPUT);

    printf("\n The non_char sample is predicted as: %f (with number of votes = %f)\n", prediction, votes);
    printf(" Class probability (using Logistic Correction) is P(r|character) = %f\n\n",
           (float) 1 - (float) 1 / (1 + exp(-2 * votes)));

// Save the trained classifierNM1
    boost->save(string(modelPath));
}