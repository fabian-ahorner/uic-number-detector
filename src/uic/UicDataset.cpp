//
// Created by fab on 10.10.16.
//

#include <dirent.h>
#include <regex>
#include <iostream>
#include <fstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <ImageUtils.h>
#include "UicDataset.h"

namespace cv {
    namespace uic {

        UicEntry::UicEntry(const string &path, const string &name) : name(name), path(path) {
            this->loadDataFile();
        }

        void UicEntry::loadDataFile() {
            ifstream in(path + "/originals/" + name + "/data.txt");
            in >> size.width;
            in >> size.height;
            std::getline(in, timeRecorded);
            std::getline(in, timeRecorded);
            std::getline(in, cameraType);
//            in >> timeRecorded;
//            in >> cameraType;
            readRectangle(in, rectTrain);
            readRectangle(in, rectUic);
            in >> uic;
            in >> quality;

            int digitBoxesCnt;
            if (in >> digitBoxesCnt) {
                for (int i = 0; i < digitBoxesCnt; ++i) {
                    cv::Rect_<double> r;
                    readRectangle(in, r);
                    digitBoxes.push_back(r);
                    if (i == 0) {
                        rectDigits = r;
                    } else {
                        rectDigits |= r;
                    }
                }
                computeDigitRects();
                scaleAll();
            }
            in.close();
        }

        void UicEntry::scaleAll() {
            int MAX_DIGIT_SIZE = 33;
//            int digitSize = getMinDigitHeight();
//            if (digitSize > MAX_DIGIT_SIZE) {
//                double scale = MAX_DIGIT_SIZE / (double) digitSize;
//                Size newSize((int) ceil(mat.cols * scale), (int) ceil(mat.rows * scale));
//                resize(mat, result, newSize);
//                return result;
//            }
            int digitSize = getMinDigitHeight();
            if (digitSize > MAX_DIGIT_SIZE) {
                scale = MAX_DIGIT_SIZE / (double) digitSize;
                size = ImageUtils::scale(size, scale);
                rectTrain = ImageUtils::scale(rectTrain, scale);
                rectUic = ImageUtils::scale(rectUic, scale);
                rectDigits = ImageUtils::scale(rectDigits, scale);
                for (int i = 0; i < digitBoxes.size(); ++i) {
                    digitBoxes[i] = ImageUtils::scale(digitBoxes[i], scale);
                }
                for (int i = 0; i < digitRects.size(); ++i) {
                    digitRects[i] = ImageUtils::scale(digitRects[i], scale);
                }
            }
        }

        void UicEntry::readRectangle(ifstream &in, Rect_<double> &rect) {
            in >> rect.x;
            in.seekg(1, ios_base::cur); // skip ","
            in >> rect.y;
            in.seekg(1, ios_base::cur);
            in >> rect.width;
            in.seekg(1, ios_base::cur);
            in >> rect.height;
        }

        const string UicEntry::getOriginalImage() const {
            return path + "/originals/" + name + "/original.jpg";
        }

        const string UicEntry::getTrainImage() const {
            return path + "/train/" + name + ".jpg";
        }

        const string UicEntry::getUicImage() const {
            return path + "/uic/" + name + ".jpg";
        }

        string UicEntry::getImagePath(UicEntry::ImageType type) const {
            switch (type) {
                case UIC:
                    return getUicImage();
                case TRAIN:
                    return getTrainImage();
                case ORIGINAL:
                    return getOriginalImage();
                default:
                    return "";
            }
        }

        Mat UicEntry::getImage(UicEntry::ImageType type) const {
            Mat img;
            switch (type) {
                case DIGITS:
                    img = imread(getUicImage());
                    resize(img, img, rectUic.size());
                    img = img(translate(rectDigits, ImageType::UIC));
                    break;
                case UIC:
                    img = imread(getUicImage());
                    resize(img, img, rectUic.size());
//                    cvtColor(img,img,CV_GRAY2BGR);
                    break;
                case TRAIN:
                    img = imread(getTrainImage());
                    resize(img, img, rectTrain.size());
                    break;
                case ORIGINAL:
                default:
                    img = imread(getOriginalImage());
                    resize(img, img, size);
                    break;
            }
            return img;
        }

        Mat UicEntry::preProcess(const Mat &mat) const {
//            Mat result;
//            Mat ycrcb;
//            cvtColor(mat, ycrcb, CV_BGR2YCrCb);
//            vector<Mat> channels;
//            split(ycrcb, channels);
//            equalizeHist(channels[0], channels[0]);
//            merge(channels, ycrcb);
//            cvtColor(ycrcb, result, CV_YCrCb2BGR);
//            return result;

//            int MAX_DIGIT_SIZE = 40;
//            int digitSize = getMinDigitHeight();
//            if (digitSize > MAX_DIGIT_SIZE) {
//                double scale = MAX_DIGIT_SIZE / (double) digitSize;
//                Size newSize((int) ceil(mat.cols * scale), (int) ceil(mat.rows * scale));
//                resize(mat, result, newSize);
//                return result;
//            }
            return mat;
        }

        Rect UicEntry::getUicBox(UicEntry::ImageType type) const {
            return translate(rectUic, type);
        }
        Rect UicEntry::getDigitBox(UicEntry::ImageType type) const {
            return translate(rectDigits, type);
        }

        UicDataset UicDatasetHelper::load(string path) {
            vector<UicEntry> entries;
            DIR *dir;
            struct dirent *ent;
            regex regexUic("[[:digit:]]{12}_[[:digit:]]+");
            string originals = path + "/originals";

            if ((dir = opendir(originals.c_str())) == NULL) {
                perror((string("Could not open dataset") + originals.c_str()).c_str());
                throw (string("Could not open dataset") + originals.c_str()).c_str();
            }
            int cnt = 0;
            while ((ent = readdir(dir)) != NULL) {
                if (regex_match(ent->d_name, regexUic)) {
                    string name = ent->d_name;
                    UicEntry *uic = new UicEntry(path, name);
//                    if(uic->getMinDigitHeight()>=33){
//                        entries.push_back(*uic);
//                    }
                    if (uic->quality < 3)
                        entries.push_back(*uic);
//                    if(cnt++>40)
//                        break;
                }
            }
            closedir(dir);
            // Sort uics to make the order platform independent
            return sortByName(entries);
        }

        UicDataset UicDatasetHelper::splitDataset(const UicDataset &dataset, double from, double to) {
            int iFrom = (int) (dataset.size() * from);
            int iTo = (int) (dataset.size() * to);
            return UicDataset(dataset.begin() + iFrom, dataset.begin() + iTo);
        }

        UicDataset UicDatasetHelper::loadAndShuffle(string path) {
            auto engine = std::default_random_engine(0);
            UicDataset dataset = load(path);
            std::shuffle(std::begin(dataset), std::end(dataset), engine);
            return dataset;
        }

        UicDataset UicDatasetHelper::sortByName(const UicDataset &dataset) {
            UicDataset sorted(dataset.begin(), dataset.end());
            sort(sorted.begin(), sorted.end(), UicEntry::sortByName);
            return sorted;
        }

        void UicEntry::computeDigitRects() {
            addDigitsFor(digitBoxes[0], 2);
            addDigitsFor(digitBoxes[1], 2);
            int totalWidth = 0;
            for (int i = 2; i < digitBoxes.size(); ++i) {
                totalWidth += digitBoxes[i].width;
            }
            int digitW = totalWidth / 8;
            int totalDigits = 0;
            for (int i = 2; i < digitBoxes.size(); ++i) {
                int count = (int) (0.5 + digitBoxes[i].width / digitW);
                if (count <= 0)
                    count = 1;
                totalDigits += count;
//                addDigitsFor(digitBoxes[i], count);
            }
            if (totalDigits == 8) {
                for (int i = 2; i < digitBoxes.size(); ++i) {
                    int count = (int) (0.5 + digitBoxes[i].width / digitW);
                    if (count <= 0)
                        count = 1;
                    addDigitsFor(digitBoxes[i], count);
                }
            } else {
                totalDigits = 0;
                for (int i = 2; i < digitBoxes.size(); ++i) {
                    int count = (int) (0.5 + digitBoxes[i].width / digitW);
                    if (count <= 0)
                        count = 1;
                    if (i == 2 && count > 1) {
                        count--;
                    }
                    totalDigits += count;
                    addDigitsFor(digitBoxes[i], count);
                }
            }
            if (totalDigits > 8)
                cout << "Too many digitRects: " << uic << endl;
            assert(totalDigits == 8);
        }

        void UicEntry::addDigitsFor(Rect_<double> &digitBox, int digits) {
            double w = digitBox.width / digits;
            for (int i = 0; i < digits; ++i) {
                Rect_<double> digit(digitBox.x + w * i, digitBox.y, w, digitBox.height);
                this->digitRects.push_back(digit);
            }
        }

        Rect_<double> UicEntry::translate(Rect_<double> rect, UicEntry::ImageType mode) const {
            switch (mode) {
                case UicEntry::UIC:
                    return rect - rectUic.tl();
                case UicEntry::TRAIN:
                    return rect - rectTrain.tl();
                case UicEntry::DIGITS:
                    return rect - rectDigits.tl();
                case UicEntry::ORIGINAL:
                default:
                    return rect;
            }
        }

        int UicEntry::getMinDigitHeight() const {
            double minHeight = rectDigits.height;
            for (Rect box:digitBoxes) {
                if (box.height < minHeight)
                    minHeight = box.height;
            }
            return (int) minHeight;
        }

        Scalar UicEntry::getDigitRMS() const {
            Mat img = imread(getUicImage());
            img = img(translate(rectDigits, ImageType::UIC));
            Mat mask = Mat::zeros(img.rows, img.cols, CV_8UC1);
//            img.convertTo(img, CV_32FC1); // or CV_32F works (too)

            for (Rect box:digitBoxes) {
                rectangle(mask, translate(box, ImageType::DIGITS), Scalar(255), -1);
            }
//            imshow("test",img&mask);
//            imshow("test2",mask);
            Scalar mean, sd;
            meanStdDev(img, mean, sd, mask);
            return sd;
        }

        Rect UicEntry::getRect(UicEntry::ImageType mode) const {
            switch (mode) {
                case UicEntry::UIC:
                    return rectUic;
                case UicEntry::TRAIN:
                    return rectTrain;
                case UicEntry::DIGITS:
                    return rectDigits;
                case UicEntry::ORIGINAL:
                default:
                    return Rect(0, 0, (int) size.width, (int) size.height);
            }
        }


    }
}