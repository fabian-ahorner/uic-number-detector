//
// Created by fab on 11.01.17.
//

#include <opencv/cv.hpp>
#include "UicEvaluator.h"
#include "UicUtils.h"

namespace cv {
    namespace uic {

        void UicEvaluator::evaluateUicStrings(PUicProcess detector, const UicDataset &dataset,
                                              UicEntry::ImageType mode, vector<string> whitelist) {
            TimeLogger::clear();
            TimeLogger::logLevel = 0;
            int editDistanceCnt[13];
            for (int j = 0; j < 13; ++j) {
                editDistanceCnt[j] = 0;
            }
            int overlapCnt[3];
            overlapCnt[0] = overlapCnt[1] = overlapCnt[2] = 0;

            int matchesSum = 0;
            float minOverlap = 0.3;
            long long int totalTime = 0;
            int criticalErrors = 0;
            TimeLogger::clear();
            for (int i = 0; i < dataset.size(); ++i) {
                Mat image = dataset[i].getImage(mode);
//                cvtColor(image, image, COLOR_BGR2GRAY);
//                cvtColor(image, image, COLOR_GRAY2BGR);
                long long int startTime = TimeUtils::MiliseconsNow();
                const PUicResultVector &pUics = detector->runWithLAB(image);
                const UicResultVector &uics = *pUics;
                long long int endTime = TimeUtils::MiliseconsNow();
                totalTime += endTime - startTime;


                Rect uicBox = dataset[i].getDigitBox(mode);

                int minEditDistance = 100;
                float maxOverlap = 0;
//                for (auto uic:uics) {
                if (uics.size() > 0) {
                    PUicResult uic = uics[0];

                    string whitelisted = UicUtils::checkWhitelist(uic->uic, whitelist);

                    int m = (int) UicUtils::editDistance(dataset[i].uic, whitelisted);
                    if (UicUtils::isValid(whitelisted) && m > 0) {
                        criticalErrors++;
                    }
                    float overlap = getOverlap(uic->getBox(), uicBox);
                    minEditDistance = MIN(minEditDistance, m);
                    maxOverlap = MAX(maxOverlap, overlap);

                    std::stringstream ss;
                    ss << dataset[i].name << "_" << m ;
                    UicDebugHelper::save(UicDebugHelper::drawUicResult(image, uic), ss.str());
                }
                if (maxOverlap < minOverlap) {
                    overlapCnt[0]++;
                } else if (maxOverlap < 0.50) {
                    overlapCnt[1]++;
                    editDistanceCnt[minEditDistance]++;
                    matchesSum += minEditDistance;
                } else {
                    overlapCnt[2]++;
                    editDistanceCnt[minEditDistance]++;
                    matchesSum += minEditDistance;
                }
                printf("%.2f%% : %3d %s -> Overlap: %3.2f%% Edit Distance: %d \r\n", (i * 100. / dataset.size()), i,
                       dataset[i].name.c_str(),
                       maxOverlap * 100, minEditDistance > 13 ? -1 : minEditDistance);
            }
            printf("\r\nDetector: %s\r\n", detector->toString().c_str());
            printf("Total Time: %d\r\nMean Time: %d\r\n", (int) totalTime, (int) (totalTime / dataset.size()));
            UicDebugHelper::printTimings(detector);

            printf("\r\n%10s\t %10s\t %10s\t\r\n", "Overlap", "Cnt", "%");
            for (int j = 0; j < 3; ++j) {
                printf("%9s%%\t %10d\t %9.2f%%\r\n", j == 0 ? "<30" : j == 1 ? ">30" : ">50", overlapCnt[j],
                       overlapCnt[j] * 100. / dataset.size());
            }

            int foundUics = overlapCnt[1] + overlapCnt[2];
            double cummulated = 0;
            double area = 0;
            printf("\r\n%10s\t %10s\t %10s\t %10s\r\n", "EditDistance", "Cnt.", "%", "Cumulated");
            for (int j = 0; j < 13; ++j) {
                cummulated += editDistanceCnt[j] * 100. / dataset.size();
                area += cummulated;
                printf("%10d\t %10d\t %9.2f%%\t %9.2f%%\r\n", j, editDistanceCnt[j],
                       editDistanceCnt[j] * 100. / dataset.size(),
                       cummulated);
            }
            printf("%10s\t %10d\t %9.2s%%\t %9.2f%% \t\r\n", "-", overlapCnt[0], "-",
                   overlapCnt[0] * 100. / dataset.size());
            printf("Area: %9.2f%%\r\n", area / 13);
            printf("False positives: %10d  \t %9.2f%%\r\n", criticalErrors, criticalErrors * 100. / dataset.size());
            cout << "Avg (overlapping only): " << matchesSum / (float) foundUics << endl;
            cout << "Avg (all): " << (matchesSum + overlapCnt[0] * 12) / (float) dataset.size() << endl;
        }

        void UicEvaluator::evaluateCharDetector(PCharProcess detector, const UicDataset &dataset,
                                                UicEntry::ImageType mode) {
            TimeLogger::clear();
            TimeLogger::logLevel = 0;

            int totalError = 0;
            int totalHits = 0;
            int totalRepeatedHits = 0;
            long long int totalTime = 0;
            int criticalErrors = 0;

            int totalPrecision = 0;
            int totalRecall = 0;
            int totalQuality = 0;

            for (int i = 0; i < dataset.size(); ++i) {
                Mat image = dataset[i].getImage(mode);
//                cvtColor(image, image, COLOR_BGR2GRAY);
//                cvtColor(image, image, COLOR_GRAY2BGR);

                long long int startTime = TimeUtils::MiliseconsNow();
                PCharVector chars = detector->runWithLAB(image);
                long long int endTime = TimeUtils::MiliseconsNow();
                totalTime += endTime - startTime;

                const UicEntry &uic = dataset[i];
                vector<Rect> estimatedRects = getRects(chars);
                vector<Rect> targetRects = getTargetRects(uic, mode);

                double precision2 = getScore(estimatedRects, targetRects);
                double recall2 = getScore(targetRects, estimatedRects);
                double f2 = 2 / (1 / precision2 + 1 / recall2);
                printf("-%.2f%% : %3d %s -> Precision: %3.2f%% Recall: %3.2f%% f: %3.2f%%\n",
                       (i * 100. / dataset.size()), i,
                       dataset[i].name.c_str(),
                       precision2 * 100, recall2 * 100, f2 * 100);
            }
            printf("\r\nDetector: %s\r\n", detector->toString().c_str());
            printf("Total Time: %d\r\nMean Time: %d\r\n", (int) totalTime, (int) (totalTime / dataset.size()));
            double recall = 100. * totalHits / (12. * dataset.size());
            double precision = 100. * totalHits / (totalHits + totalError + totalRepeatedHits);
            double repeatedPrecision =
                    100. * (totalHits + totalRepeatedHits) / (totalHits + totalError + totalRepeatedHits);
            double redundancy = 100. * totalRepeatedHits / totalHits;
            double f1 = 2 * precision * recall / (precision + recall);
            double f1Repeated = 2 * repeatedPrecision * recall / (repeatedPrecision + recall);
            printf("Recall: %5.2f%% Precision: %5.2f%% Repeated-Precision: %5.2f%% Ø-Errors: %5d Redundancy: %5.2f%% F1: %3.2f Repeated-F1: %3.2f\r\n",
                   recall, precision, repeatedPrecision, totalError / (int) dataset.size(), redundancy, f1, f1Repeated);
        }

        int UicEvaluator::countMatches(string uic, vector<TextLine> lines) {
            int matches = 0;
            for (int i = 0; i < lines.size(); ++i) {
                const string &word = lines[i].getPrediction();
                for (int j = 0; j < word.length(); ++j) {
                    unsigned long pos = uic.find(word[j]);
                    if (pos != std::string::npos) {
                        matches++;
                        uic[pos] = 0;
                    }
                }
            }
            return matches;
        }

        float UicEvaluator::getOverlap(const Rect &r1, const Rect &r2) {
            return (r1 & r2).area() / (float) (r1 | r2).area();
        }

        void UicEvaluator::evaluateResult(const UicEntry &original, const PUicResultVector &uicResults,
                                          UicEntry::ImageType mode) {
            cout << "UICs: " << endl;
            int i = 0;
            for (auto uic:*uicResults) {
//            if (!uicResults.empty()) {
//                const UicResult &uic = *(*uicResults)[0];
                int editDistance = (int) UicUtils::editDistance(uic->uic, original.uic);
                Rect resultRect = uic->getBox();
                float overlap = getOverlap(resultRect, original.getDigitBox(mode));
                printf("%i  %12s: Edit Distance=%2d Overlapp=%3.2f%% score=%f %d %d %d\n", i, uic->uic.c_str(),
                       editDistance,
                       overlap * 100, uic->getScore(), uic->lineStats[0].validCharacters,
                       uic->lineStats[1].validCharacters, uic->lineStats[2].validCharacters);
                uic->update();
                if (i++ > 5)
                    break;
            }
            cout << endl;
        }

        vector<Rect> UicEvaluator::getRects(PCharVector chars) {
            vector<Rect> rects;
            for (auto ch:*chars) {
                rects.push_back(ch->rect);
            }
            return rects;
        }

        vector<Rect> UicEvaluator::getTargetRects(const UicEntry &uic, UicEntry::ImageType mode) {
            vector<Rect> rects;
            for (auto ch:uic.digitRects) {
                rects.push_back(uic.translate(ch, mode));
            }
            return rects;
        }

        double UicEvaluator::getScore(vector<Rect> rects1, vector<Rect> rects2) {
            double total = 0;
            for (auto rect:rects1) {
                total += getMaxOverlap(rect, rects2);
            }
            return total / rects1.size();
        }

        double UicEvaluator::getMaxOverlap(Rect target, vector<Rect> rects) {
            double maxOverlapp = 0;
            for (auto rect:rects) {
                maxOverlapp = MAX(maxOverlapp, getOverlap(target, rect));
            }
            return maxOverlapp;
        }
    }
}