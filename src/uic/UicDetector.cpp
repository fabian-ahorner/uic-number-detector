//
// Created by fab on 18.11.16.
//

#include "UicDetector.h"
#include "../utils/ParallelUtils.cpp"
#include "UicUtils.h"
#include "UicDebugHelper.h"


#define CHARACTER_SPACING 1.5f
#define MAX_HEIGHT_DIFF 0.5f
#define MAX_CHARS_PER_LINE 8
namespace cv {
    namespace uic {


        PUicResultVector SimpleUicDetector::process(Mat &original, Mat &img, PLineVector lines) {
            std::sort(lines->begin(), lines->end(), TextLine::sortByY);
            PUicResultVector uics = new UicResultVector();
            LineVector matchingLines;
            for (int l = 0; l < lines->size(); l++) {
                matchingLines.clear();
                PTextLine &line = (*lines)[l];
//                matchingLines.push_back(&line);

                maxHeight = line->getAvgHeight() + MAX_HEIGHT_DIFF * line->getAvgHeight();
                minHeight = line->getAvgHeight() - MAX_HEIGHT_DIFF * line->getAvgHeight();

                Rect searchArea = getSearchArea(line);
                extractUics(img, lines, searchArea, l, matchingLines, uics);
            }
            if (skipOCR)
                doOCR(original, uics);
            std::sort(uics->begin(), uics->end(), UicDetector::sortByScore);
            if (uics->size() > 0) {
                auto best = (*uics)[0];
                best->update();
            }
            return uics;
        }

        void
        SimpleUicDetector::extractUics(Mat &original, PLineVector &lines, Rect searchArea, int iNextLine,
                                       LineVector matchingLines, PUicResultVector &uics) {
            matchingLines.push_back((*lines)[iNextLine]);
            if (matchingLines.size() < 3) {
                updateSearchArea(searchArea, (*lines)[iNextLine]);
                int maxY = searchArea.y + searchArea.height;
                for (iNextLine = iNextLine + 1; iNextLine < lines->size(); iNextLine++) {
                    const PTextLine &possibleNextLine = (*lines)[iNextLine];
                    if (searchArea.contains(possibleNextLine->rect.tl())
                        && possibleNextLine->getAvgHeight() > minHeight
                        && possibleNextLine->getAvgHeight() < maxHeight) {
                        extractUics(original, lines, searchArea, iNextLine, matchingLines, uics);
//                        break;
                    } else if (possibleNextLine->rect.y > maxY) {
                        break;
                    }
                }
            } else {
                PUicResult uic = extractUICFromLines(original, matchingLines);
                addResult(uics, uic);
//                uics->push_back(uic);
            }
        }

        void SimpleUicDetector::addResult(PUicResultVector &uics, PUicResult uic) {
            if (uics->empty()) {
                uics->push_back(uic);
            } else {
                double currentScore = (*uics)[0]->getScore();
                double uicScore = uic->getScore();
                if (currentScore < uicScore)
                    uics->clear();
                if (currentScore <= uicScore)
                    uics->push_back(uic);
            }
        }

        Rect SimpleUicDetector::getSearchArea(const PTextLine &line) {
            float missingChars = MAX(2,
                                     MAX_CHARS_PER_LINE -
                                     line->rect.width / (CHARACTER_SPACING * line->getAvgWidth()));
            int missingWidth = (int) (missingChars * CHARACTER_SPACING * line->getAvgWidth());
            int minX = line->rect.x - missingWidth;
            int maxX = line->rect.x + line->rect.width + missingWidth;

            Rect searchArea(minX, 0, maxX - minX, 0);
            updateSearchArea(searchArea, line);
            return searchArea;
        }

        void SimpleUicDetector::updateSearchArea(Rect &searchArea, const PTextLine &line) {
            int minY = line->rect.y + (int) line->getAvgHeight();
            int maxY = line->rect.y + (int) (2 * line->getAvgHeight());
            searchArea.y = minY;
            searchArea.height = maxY - minY;
        }

        PUicResult SimpleUicDetector::extractUICFromLines(Mat &original, LineVector &lines) {
            return new UicResult(lines[0], lines[1], lines[2]);
        }

        void SimpleUicDetector::doOCR(Mat &original, PUicResultVector &uics) {
            TimeLogger::logEnd(this->toString());
            TimeLogger::logStart(textDetector->toString());
            for (auto &uic: *uics) {
                for (auto &line: uic->lines) {
                    if (!line->hasPrediction()) {
                        textDetector->classifyLine(original, line);
                    }
                }
                uic->update();
            }
            TimeLogger::logEnd(textDetector->toString());
            TimeLogger::logStart(this->toString());
        }

        bool UicResult::isValid() const {
            return UicUtils::isValid(uic);
        }

        UicResult::UicResult() {
        }

        int UicResult::getLineOffset(int leftMostX, const PTextLine &line) {
            double avgCharWidth = line->getAvgWidth();
            return (int) MAX(0, round((line->getBoundingBox().x - leftMostX) / (CHARACTER_SPACING * avgCharWidth)));
        }

        UicResult::UicResult(const PTextLine &line1, const PTextLine &line2, const PTextLine &line3) : lines{
                line1, line2, line3} {
            update();
        }

        int UicResult::countNonDigits(const string &str, int length) {
            int nonDigits = 0;
            for (int i = 0; i < str.length() && i < length; ++i) {
                if (str[i] < '0' || str[i] > '9')
                    nonDigits++;
            }
            return nonDigits;
        }

        double UicResult::getConfidence(const PTextLine &line, int length) {
            if (length == 0)
                return 0;
            double confidence = 0;
            const PCharVector &chars = line->getCharacters();
            for (int i = 0; i < length && i < chars->size(); ++i) {
                Ptr<CharacterCandidate> &ch = (*chars)[i];
                char prediction = ch->getPrediction();
                if (isDigit(prediction)) {
                    confidence += ch->getConfidence() / 100;
                }
            }
            return confidence;
        }

        bool UicResult::isDigit(char character) {
            return character > '0' && character < '9';
        }

        LineStats UicResult::calculateStats(int leftMostX, const PTextLine &line, int maxLength) {
            LineStats stats;
            stats.availableCharacters = MIN(maxLength, line->size());
            stats.offset = MAX(0, maxLength - getLineOffset(leftMostX, line));
            stats.validCharacters = MIN(stats.availableCharacters, stats.offset);

            if (line->hasPrediction()) {
                stats.nonDigits = countNonDigits(line->getPrediction(), stats.validCharacters);
                double confidence = getConfidence(line, stats.validCharacters);
                stats.digitConfidence = confidence / maxLength;
                stats.validCharacters -= stats.nonDigits;
            }
            return stats;
        }

        void UicResult::update() {
            PTextLine &line1 = lines[0];
            PTextLine &line2 = lines[1];
            PTextLine &line3 = lines[2];
            string uicLine1 = line1->getPrediction().substr(0, 2);
            string uicLine2 = line2->getPrediction().substr(0, 2);
            string uicLine3 = line3->getPrediction().substr(0, 8);
            this->uic = uicLine1 + uicLine2 + uicLine3;
            int leftMostX = MIN(line1->getBoundingBox().x,
                                MIN(line2->getBoundingBox().x, line3->getBoundingBox().x));
            lineStats[0] = calculateStats(leftMostX, lines[0], 2);
            lineStats[1] = calculateStats(leftMostX, lines[1], 2);
            lineStats[2] = calculateStats(leftMostX, lines[2], 8);
            this->score =
                    lineStats[0].getScore(2 / 12.) + lineStats[1].getScore(2 / 12.) + lineStats[2].getScore(8 / 12.) +
                    (isValid() ? 1 : 0);
        }

        Mat UicResult::preprocessLine(int line, Mat img) {
            Mat result;
            Mat mask(img.rows, img.cols, CV_8UC1);
            UicDebugHelper::drawLine(mask, lines[line]);
            mask = mask(lines[line]->getBoundingBox());
            Mat in = img(lines[line]->getBoundingBox());


            Mat bgdModel, fgdModel;
            Mat foreground;
            Mat background;
            int erodeSize = in.rows / 15;
            Mat large = getStructuringElement(MORPH_ELLIPSE,
                                              Size(2 * erodeSize + 1, 2 * erodeSize + 1),
                                              Point(erodeSize, erodeSize));
            Mat px1 = getStructuringElement(MORPH_ELLIPSE,
                                            Size(2 * 1 + 1, 2 * 1 + 1),
                                            Point(1, 1));
            threshold(mask, mask, 128, 255, 0);
            Mat dilated;
            dilate(mask, dilated, large);
            dilate(dilated, background, px1);
            background -= dilated;
            Mat segmentation(in.rows, in.cols, CV_8UC1);
            segmentation.setTo(GC_PR_BGD);
            segmentation.setTo(GC_FGD, mask);
            segmentation.setTo(GC_BGD, background);
            grabCut(in, segmentation, Rect(), bgdModel, fgdModel, 3, GC_INIT_WITH_MASK);

            return segmentation * 50;
        }

        PUicResultVector ChannelUicDetector::processImage(Mat original, Mat img) {
            vector<Mat> channels;
            PUicResultVector results = new UicResultVector();

            ImageUtils::splitInChannels(img, channels, mode);

//            Mat lab;
//            cvtColor(img, lab, COLOR_RGB2Lab);
//            getChildPipeline()->getChildPipeline()->setOriginal(lab);
            for (Mat &channel:channels) {
                PUicResultVector result = SimpleUicDetector::processImage(original, channel);
                results->insert(results->end(), result->begin(), result->end());
            }
            std::sort(results->begin(), results->end(), sortByScore);
            return results;
        }
    }
}