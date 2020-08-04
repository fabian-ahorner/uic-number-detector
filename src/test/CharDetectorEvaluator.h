//
// Created by fab on 03.07.17.
//

#ifndef UIC_CHARDETECTOREVALUATOR_H
#define UIC_CHARDETECTOREVALUATOR_H

#include <CharacterDetector.h>
#include <TextDetector.h>
#include <opencv/cv.hpp>
#include "../uic/UicDataset.h"
#include "../uic/UicDebugHelper.h"
#include "../char/CharacterDetector.h"

namespace cv {
    namespace text {
        using namespace uic;

        struct CharDetectorReport {
            struct Entry {
                double recall, precision, f, f2, ocrQuality;
                int time, chars;
            };
            vector<Entry> entries;
            vector<double> values;
            int maxTime = 0;

            inline void add(double value, Entry entry) {
                values.push_back(value);
                entries.push_back(entry);
                maxTime = MAX(entry.time, maxTime);
            }

            inline void toCSV() {
                cout << "value, precision, recall, f, f2, ocr, time, chars" << endl;
                for (int i = 0; i < values.size(); i++) {
                    Entry e = entries[i];
                    printf("%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %d, %d\r\n", values[i], e.precision * 100,
                           e.recall * 100, e.f * 100, e.f2 * 100,
                           e.ocrQuality * 100, e.time, e.chars);
                }
            }
        };

        class CharDetectorEvaluator {
        public:
            CharDetectorEvaluator(UicDataset dataset, Ptr<TesseractSingleCharClassifier> ocr = Ptr<TesseractSingleCharClassifier>()) {
                this->dataset = dataset;
                this->ocr = ocr;
                this->minOcrOverlap = 0.5;
                this->mode = UicEntry::ImageType::UIC;
            }


            inline CharDetectorReport::Entry evaluateCharDetector(PCharProcess detector) {
                TimeLogger::clear();
                TimeLogger::logLevel = 0;

                int totalError = 0;
                int totalHits = 0;
                int totalRepeatedHits = 0;
                long long int totalTime = 0;
                int criticalErrors = 0;

                double totalPrecision = 0;
                double totalRecall = 0;
                double totalF = 0;
                double totalQuality = 0;
                int qualityCnt = 0;
                int charCount = 0;
                OcrScore ocrScore(0, 0);
                PCharVector allChars = new CharVector();
                for (int i = 0; i < dataset.size(); ++i) {
                    const UicEntry &uic = dataset[i];
                    Mat image = uic.getImage(mode);

                    cout << "=";
                    cout << flush;
                    long long int startTime = TimeUtils::MiliseconsNow();
                    PCharVector chars = detector->runWithLAB(image);
                    long long int endTime = TimeUtils::MiliseconsNow();
                    totalTime += endTime - startTime;

                    vector<Rect> estimatedRects = getRects(chars);
                    vector<Rect> targetRects = getTargetRects(uic, mode);

                    charCount += chars->size();
                    double precision = getScore(estimatedRects, targetRects);
                    double recall = getScore(targetRects, estimatedRects);
                    double f = 2 / (1 / precision + 1 / recall);
                    ocrScore += getOcrScore(image, chars, targetRects, uic.uic);
//                    printf("> %.2f%% : %3d %s -> Precision: %3.2f%% Recall: %3.2f%% f: %3.2f%% OCR-Quality: %3.2f\n",
//                           (i * 100. / dataset.size()), i,
//                           dataset[i].name.c_str(),
//                           precision * 100, recall * 100, f * 100, ocrQuality * 100);

                    totalPrecision += precision;
                    totalRecall += recall;
                    totalF += f;

                    allChars->insert(std::end(*allChars), std::begin(*chars), std::end(*chars));

//                    UicDebugHelper::save(image);
                }
                UicDebugHelper::save(UicDebugHelper::drawRandomCharsWithPrediction(allChars), std::__cxx11::string());
//                UicDebugHelper::saveChars(allChars, "/home/fab/datasets/char/uic_1/");
                printf("\r\nDetector: %s\r\n", detector->toString().c_str());
                printf("Total Time: %d\r\nMean Time: %d\r\n", (int) totalTime, (int) (totalTime / dataset.size()));

                double meanPrecision = totalPrecision / dataset.size();
                double meanRecall = totalRecall / dataset.size();
                double meanF = totalF / dataset.size();
//                double meanQuality = totalQuality / qualityCnt;
                double meanChars = totalQuality / qualityCnt;

                printf("Recall: %5.2f%% Precision: %5.2f%% F1: %3.2f OCR-Quality: %3.2f / %d\r\n",
                       meanRecall * 100, meanPrecision * 100, meanF * 100, ocrScore.getScore() * 100, ocrScore.total);

                CharDetectorReport::Entry report;
                report.precision = meanPrecision;
                report.recall = meanRecall;
                report.f = meanF;
                report.f2 = (1 + 2 * 2) * meanPrecision * meanRecall / (2 * 2 * meanPrecision + meanRecall);
                report.ocrQuality = ocrScore.getScore();
                report.time = (int) (totalTime / dataset.size());
                report.chars = (int) (charCount / dataset.size());

                return report;
            }

            inline vector<Rect> getRects(PCharVector chars) {
                vector<Rect> rects;
                for (auto ch:*chars) {
                    rects.push_back(ch->rect);
                }
                return rects;
            }

            inline vector<Rect> getTargetRects(const UicEntry &uic, UicEntry::ImageType mode) {
                vector<Rect> rects;
                for (auto ch:uic.digitRects) {
                    rects.push_back(uic.translate(ch, mode));
                }
                return rects;
            }

            inline double getScore(vector<Rect> rects1, vector<Rect> rects2) {
                double total = 0;
                for (auto rect:rects1) {
                    total += getMaxOverlap(rect, rects2);
                }
                return rects1.empty() ? 0 : total / rects1.size();
            }

            inline double getMaxOverlap(Rect target, vector<Rect> rects) {
                double maxOverlapp = 0;
                for (auto rect:rects) {
                    maxOverlapp = MAX(maxOverlapp, getOverlap(target, rect));
                }
                return maxOverlapp;
            }

            inline float getOverlap(const Rect &r1, const Rect &r2) {
                return (r1 & r2).area() / (float) (r1 | r2).area();
            }

            struct OcrScore {
                int total;
                int successful;

                OcrScore(int successful, int total) {
                    this->total = total;
                    this->successful = successful;
                }

                OcrScore operator+(const OcrScore &s) {
                    return OcrScore(successful + s.successful, total + s.total);
                }

                OcrScore &operator+=(const OcrScore &s) {
                    this->successful = successful + s.successful;
                    this->total = total + s.total;
                    return *this;
                }

                double getScore() {
                    return total == 0 ? 0 : successful / (double) total;
                }
            };

            inline OcrScore getOcrScore(Mat &img, PCharVector &estimates, vector<Rect> &targets, const String &uic) {
                double recognised = 0;
                double tried = 0;
                for (auto &ch:*estimates) {
                    double maxOverlap = 0;
                    int maxIndex = -1;
                    for (int i = 0; i < targets.size(); i++) {
                        double overlap = getOverlap(ch->rect, targets[i]);
                        if (overlap > minOcrOverlap && overlap > maxOverlap) {
                            maxOverlap = overlap;
                            maxIndex = i;
                        }
                    }
                    if (maxIndex >= 0) {
                        tried++;
                        char prediction;
                        if (ch->prediction != 0) {
                            prediction = ch->getPrediction();
                        } else {
                            cout << "OCR!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                            ocr->clearAdaptiveClassifier();
                            prediction = ocr->classifyChar(img, ch);
                        }
                        if (prediction == uic[maxIndex])
                            recognised++;
                        ch->setPrediction(prediction, prediction == uic[maxIndex] ? 1 : -1);
                    } else {
                        ch->setPrediction(0);
                    }
                }
                return OcrScore(recognised, tried);
            }

            UicDataset dataset;
            Ptr<TesseractSingleCharClassifier> ocr;
            double minOcrOverlap;
            UicEntry::ImageType mode;
        };
    }
}


#endif //UIC_CHARDETECTOREVALUATOR_H
