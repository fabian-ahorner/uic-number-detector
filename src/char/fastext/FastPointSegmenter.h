//
// Created by fab on 26.11.16.
//

#ifndef UIC_FPOINTSEGMENTER_H
#define UIC_FPOINTSEGMENTER_H

#include <opencv2/core/core.hpp>
#include "FastPoint.h"
#include "FastCandidate.h"
#include "NeighbourVisitor.h"
#include "FastPointFactory.h"
#include "../CharacterCandidate.h"
#include "FasTextOptions.h"
#include <opencv2/ml.hpp>
#include <math.h>
namespace cv {
    namespace text {
        using namespace ml;

        template<class VAL>
        struct PixelToCheck {
            const VAL *const imgPtr;
            char *const maskPtr;
            Point point;

            PixelToCheck(const VAL *imgPtr, char *maskPtr, const Point &point)
                    : imgPtr(imgPtr), maskPtr(maskPtr), point(point) {}
        };

        template<class VAL>
        class FastPointSegmenter {
            const FasTextOptions options;
            Ptr<StatModel> classifierNM1;
            Ptr<StatModel> classifierNM2;
            Mat img, mask;
            vector<Ptr<FastCandidate<VAL>>> candidates;
            vector<PixelToCheck<VAL>> toAdd;
            int maxWidth, maxHeight, maxArea;
            const Ptr<FastPointFactory<VAL>> factory;
        public:
            NeighbourVisitor<char> maskVisitor;
            NeighbourVisitor<char> neighbourVisitor;
            NeighbourVisitor<const VAL> imgVisitor;

            inline FastPointSegmenter(Ptr<FastPointFactory<VAL>> factory, Mat img, FasTextOptions options)
                    : factory(factory), img(img), mask(Mat::zeros(img.rows, img.cols, CV_8UC1)),
                      maskVisitor(createNeighbourVisitor<char>(mask,
                                                               NeighbourVisitor_CIRCLE8)),
                      imgVisitor(createNeighbourVisitor<const VAL>(img,
                                                                   NeighbourVisitor_CIRCLE8)),
                      neighbourVisitor(createNeighbourVisitor<char>(mask,
                                                                    FastCandidate<VAL>::NEIGHBOURS)),
                      maxWidth(MIN(15, img.cols / options.charsX)),
                      maxHeight(MIN(30, img.rows / options.charsY)),
                      maxArea(calcMaxArea(maxWidth,maxHeight)),
                      options(options) {
            }

            static inline int calcMaxArea(int w,int h){
                int strokeW = MIN(3, (int)ceil(w/3));
                int strokeH = MIN(3, (int)ceil(h/5));
                int area = strokeW * h * 2 + strokeH * ( w - strokeW*2) * 3;
                return area;
            }
//            10.00, 28.53, 75.86, 40.63, 56.96, 60.85, 210, 823
//            10.00, 28.54, 75.86, 40.63, 56.97, 60.88, 206, 822

            void segment(const Ptr<FastPoint<VAL>> point);

            inline const Mat getMask() {
                return mask;
            }

//            void drawCandidate(Mat &out, const FastCandidate &candidate, const Mat &mask);
//
//            void drawCandidate(Mat &out, const FastCandidate &candidate, const Mat &mask, const Mat &img);

            void printMat(string name, Mat mat);

            vector<Ptr<FastCandidate<VAL>>> &getCandidates() {
                return candidates;
            }

            vector<Ptr<FastCandidate<VAL>>> extractCandidates(vector<Ptr<FastPoint<VAL>>> points);

            void setClassifierNM1(Ptr<StatModel> classifier);

            void setClassifierNM2(Ptr<StatModel> classifier);
        };

        template<class VAL>
        void FastPointSegmenter<VAL>::segment(const Ptr<FastPoint<VAL>> point) {
            Ptr<FastCandidate<VAL>> candidate = factory->createFastCandidate(point);
            toAdd.clear();

            char *maskPtr = maskVisitor.ptr(point->row, point->col, mask);

            if (point->suppressed)
                return;
//            if (maskPtr[0] != 0)
//                return;
            const VAL *imgPtr = imgVisitor.ptr(point->row, point->col, img);
            Point p(point->getX(), point->getY());
            maskPtr[0] = (char) 255;

            toAdd.push_back(PixelToCheck<VAL>(imgPtr, maskPtr, p));
            Rect bounds(0, 0, img.cols, img.rows);
            do {
                PixelToCheck<VAL> pixel = toAdd.back();
                toAdd.pop_back();

                const VAL &originVal = pixel.imgPtr[0];

                for (int i = 0; i < maskVisitor.size; i++) {
                    Point p = pixel.point + maskVisitor.coordinates[i];
//                        if (maskVisitor.isValid(neighbourMaskPtr)) {
                    if (bounds.contains(p)) {
                        maskPtr = pixel.maskPtr + maskVisitor.indices[i];
                        //Check if mask has not been filled yet
                        if (maskPtr[0] == 0) {
                            imgPtr = pixel.imgPtr + imgVisitor.indices[i];
                            VAL val = imgPtr[0];
                            if (candidate->point->accepts(originVal, val)) {
                                candidate->addPixel(val, p, maskPtr, neighbourVisitor);
                                maskPtr[0] = (char) 255;
//                                printMat("Mask", mask);
                                toAdd.push_back(PixelToCheck<VAL>(imgPtr,
                                                                  maskPtr, p));
                            } else {
                                candidate->addNeighbour(val, p);
                            }
                        }
                    }
                }
//                imshow("mask", mask);
//                waitKey(0);
            } while (!toAdd.empty() && candidate->area < maxArea && candidate->getHeight() < maxHeight &&
                     candidate->getWidth() < maxWidth);//
            if (candidate->area < maxArea && candidate->getHeight() < maxHeight &&
                candidate->getWidth() < maxWidth && candidate->area > CharacterCandidate::MIN_CHAR_AREA &&
                candidate->getWidth() > 1 &&
                candidate->getHeight() > 1) {
                candidate->computeNM1Features();
                if (classifierNM1.empty()) {
                    candidates.push_back(candidate);
                } else {
                    candidate->updateProbabilityNM1(classifierNM1);
                    if (candidate->probability > options.minProbabilityNM1) {
                        if (classifierNM2.empty()) {
                            candidates.push_back(candidate);
                        } else {
                            candidate->computeNM2Features(mask);
                            candidate->updateProbabilityNM2(classifierNM2);
                            if (candidate->probability > options.minProbabilityNM2) {
                                candidates.push_back(candidate);
                            }
                        }
                    }
                }
            }
//            mask = Scalar(0);

            mask(candidate->getRect()) = Scalar(0);
//            printMat("Mask", mask);
        }

        template<class VAL>
        vector<Ptr<FastCandidate<VAL>>>
        FastPointSegmenter<VAL>::extractCandidates(vector<Ptr<FastPoint<VAL>>> points) {
            for (const Ptr<FastPoint<VAL>> &point:points) {
                segment(point);
            }
//            while (!points.empty()) {
//                const FastPoint point = points.top();
//                points.pop();
//            }
            return candidates;
        }

        template<class VAL>
        void FastPointSegmenter<VAL>::setClassifierNM1(Ptr<StatModel> classifier) {
            this->classifierNM1 = classifier;
        }

        template<class VAL>
        void FastPointSegmenter<VAL>::setClassifierNM2(Ptr<StatModel> classifier) {
            this->classifierNM2 = classifier;
        }

        template<class VAL>
        void FastPointSegmenter<VAL>::printMat(string name, Mat mat) {
            printf("%3s ", name.c_str());
            for (int col = 0; col < mat.cols; col++) {
                printf("%8x ", col);
            }
            printf("\n");
            for (int row = 0; row < mat.rows; row++) {
                printf("%3x ", row);
                for (int col = 0; col < mat.cols; col++) {
                    int val = mat.at<int>(row, col);
                    printf("%8x ", val & 0xFFFFFF);
                }
                printf("\n");
            }
        }
    }
}


#endif //UIC_FPOINTSEGMENTER_H
