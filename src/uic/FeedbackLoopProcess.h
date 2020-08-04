//
// Created by fab on 18.06.17.
//

#ifndef UIC_FEEDBACKLOOPPROCESS_H
#define UIC_FEEDBACKLOOPPROCESS_H

#include <ScaleWrapper.h>
#include <opencv/cv.hpp>
#include <../utils/Median.h>
#include "UicDetector.h"
#include "../char/MovedCharacter.h"
#include "../line/LineCandidate.h"
#include "UicDebugHelper.h"

namespace cv {
    namespace uic {
        using namespace std;

        class CharacterFilter : public WrapperImageProcess<PCharVector> {
        private:
            double width, height;
            Scalar mean;
            vector<Rect> allowedLines;

            double withinRatio(double v1, double v2, double minRatio, double maxRatio) {
                double ratio = v1 / v2;
                return ratio > minRatio && ratio < maxRatio;
            }

        public:
            CharacterFilter(PCharProcess delegate) {
                this->setChild(delegate);
            }

            void setAllowedLines(vector<Rect> &lineRects) {
                this->allowedLines = lineRects;
            }

            void reset() {
                this->allowedLines.clear();
            }

            void setWidth(double width) {
                this->width = width;
            }

            void setHeight(double height) {
                this->height = height;
            }

            void setMean(Scalar mean) {
                this->mean = mean;
            }

            inline virtual PCharVector processImage(Mat original, Mat img) {
                const PCharVector &results = child->processImage(original, img);
                if (allowedLines.empty()){
                    return results;
                }
                PCharVector filtered = new CharVector();
                for (auto it = results->begin(); it < results->end(); it++) {
                    auto ch = *it;
//                    filtered->push_back(ch);s
                    for (int i = 0; i < 3; i++) {
                        if ((allowedLines[i] & ch->rect).width > 0) {
                            if (withinRatio(ch->height, height, 0.7, 1.5)
//                                && withinRatio(ch->width, width, 0.1, 2)
//                                && norm(mean - ch->getMean()) < 100
                                    ) {
                                filtered->push_back(ch);
                            }
                            break;
                        }
                    }
                }
                return filtered;
            }

            string getProcessName() {
                return this->child->toString() + " >> CharacterFilter";
            }

        };


        class FeedbackLoopProcess : public WrapperImageProcess<PUicResultVector> {
        private:
            PUicProcess initialDetector;
            PUicProcess correctionDetector;
            Ptr<CharacterFilter> filter;

        public:
            inline FeedbackLoopProcess(PUicProcess initialDetector, PUicProcess correctionDetector,
                                       Ptr<CharacterFilter> filter) {
                this->setChild(initialDetector);

                this->initialDetector = initialDetector;
                this->correctionDetector = correctionDetector;
                this->filter = filter;
            }


            PTextLine moveLine(PTextLine line, Point move);

            PUicResultVector moveResults(PUicResultVector uics, Point move) {
                PUicResultVector results = new UicResultVector();
                for (auto uic:*uics) {
                    const PTextLine &line1 = moveLine(uic->lines[0], move);
                    const PTextLine &line2 = moveLine(uic->lines[1], move);
                    const PTextLine &line3 = moveLine(uic->lines[2], move);
                    results->push_back(new UicResult(line1, line2, line3));
                }
                return results;
            }

            inline virtual PUicResultVector processImage(Mat original, Mat img) {
                PUicResultVector results = child->processImage(original, img);
                filter->reset();
                if (results->empty())
                    return correctionDetector->processImage(original, img);

                PUicResult best = (*results)[0];
                if (best->isValid())
                    return results;

                int minX = best->getBox().x;

                Mat lineOriginal = Mat::zeros(original.rows, original.cols, original.type());
                Mat lineImg = Mat::zeros(img.rows, img.cols, img.type());

                Rect uicRect = best->getBox();
                Median medianWidth;
                Median medianHeight;
                vector<Rect> allowedLines;
                int iLine = 0;
                Scalar mean;
                for (auto line:best->lines) {
                    Rect lineRect = line->rect;

                    int margin = lineRect.height / 8;
                    lineRect.x = MAX(0, MIN(lineRect.x, minX) - margin);

                    int expectedChars = iLine == 2 ? 8 : 2;
                    lineRect.width = MIN(img.cols - lineRect.x, (int) (lineRect.height * expectedChars) + margin * 2);

                    lineRect.y = MAX(0, lineRect.y - margin);
                    lineRect.height = MIN(img.rows - lineRect.y, lineRect.height + margin * 2);
                    uicRect = uicRect | lineRect;
                    allowedLines.push_back(lineRect - uicRect.tl());


//                    if (iLine == 2) {
                    for (auto ch:*line->getCharacters()) {
                        medianHeight.add(ch->height);
                        medianWidth.add(ch->width);
//                            mean += ch->getMean();
                    }
                    mean = mean / line->size();
//                    }
                    iLine++;
                    img(lineRect).copyTo(lineImg(lineRect));
                    original(lineRect).copyTo(lineOriginal(lineRect));
                }
                filter->setAllowedLines(allowedLines);
                filter->setWidth(medianWidth.get());
                filter->setHeight(medianHeight.get());
                filter->setMean(mean);

                lineOriginal = lineOriginal(uicRect);
                lineImg = lineImg(uicRect);
//                return correctionDetector->processImage(original, img);
//                imshow("test", lineImg);
//                waitKey();
//                UicDebugHelper::show("lineOriginal", lineOriginal);
//                UicDebugHelper::show("lineImg", lineImg);
//                waitKey();
//                cout << "SECOND STAGE" << endl;
                return moveResults(correctionDetector->processImage(lineOriginal, lineImg), uicRect.tl());
//                return correctionDetector->processImage(lineOriginal(best->getBox()), lineImg(best->getBox()));
            }


            string getProcessName() {
                return this->child->toString() + " >> FeedbackLoopProcess";
            }
        };
    }
}

#endif //UIC_FEEDBACKLOOPPROCESS_H
