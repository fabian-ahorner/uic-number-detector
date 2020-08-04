//
// Created by fab on 09.12.16.
//

#include "UicDebugHelper.h"
#include <opencv2/highgui.hpp>

namespace cv {
    namespace uic {
        int UicDebugHelper::imageIndex = 0;
        string UicDebugHelper::imagePath = string("");

        PUicResultVector UicDebugHelper::computeAndShowIntermediates(Mat img, PUicProcess uicDetector) {
            Size imgSize(img.cols, img.rows);
//            Ptr<TextDetector> textDetector = uicDetector->getChildPipeline();
//            Ptr<LineDetector> lineDetector = textDetector->getChildPipeline();
//            Ptr<CharacterDetector> charDetector = lineDetector->getChildPipeline();
//
//            PCharVector chars = charDetector->processImage(img);
//            PLineVector lines = lineDetector->processStep(chars);
//            lines = textDetector->processStep(lines);
//            vector<UicResult> uics = uicDetector->processStep(lines);
//
//            show("chars", drawChars(chars, imgSize));
//            show("lines", drawLines(*lines, imgSize));
//            show("seperate lines", drawSeperateLines(*lines, imgSize));
//            show("words", drawTextPredictions(*lines, img));

            PUicResultVector uics = uicDetector->runWithLAB(img);
            PLineVector uicLines = new LineVector();
            if (uics->size() > 0) {
                Mat out;
                img.copyTo(out);
                PUicResult bestUic = (*uics)[0];
                show("uic Shape", drawUicShape(img, bestUic));
                uicLines->push_back(bestUic->lines[0]);
                uicLines->push_back(bestUic->lines[1]);
                uicLines->push_back(bestUic->lines[2]);
                show("seperate chars", drawSeperateChars(uicLines, Size(img.cols, img.rows)));
                show("uics", drawUics(img, bestUic));
//                show("line3", bestUic->preprocessLine(2,out));
            }

            printTimings(uicDetector);
            return uics;
        }

        Mat UicDebugHelper::drawUicShape(Mat &image, const PUicResult &uic) {
            Mat tmp = Mat::zeros(image.rows, image.cols, CV_WINDOW_NORMAL);
            for (const PTextLine &line: uic->lines) {
                drawLine(tmp, line);
            }
            return tmp;
        }

        void UicDebugHelper::showWords(const Mat &image, const PLineVector lines) {
            Mat textImage = drawTextPredictions(lines, image);
            namedWindow("words", WINDOW_NORMAL);
            imshow("words", textImage);
        }

        Mat UicDebugHelper::drawUics(const Mat &original, const PUicResult &uic) {
            Mat img;
            original.copyTo(img);
//            for (auto &uic:uics) {
            for (int i = 0; i < 3; i++) {
                drawWord(img, uic->lines[i]);
            }
            rectangle(img, uic->getBox().tl() - Point(4, 4), uic->getBox().br() + Point(4, 4), Scalar(0, 0, 255), 3);

            return img;
        }

        Mat UicDebugHelper::drawUicResult(const Mat &original, const PUicResult &uic) {
            Mat img;
            original.copyTo(img);
//            for (auto &uic:uics) {
            for (int i = 0; i < 3; i++) {
                drawWord(img, uic->lines[i]);
            }

            Rect r = uic->getBox();
            int b = r.height / 5;
            int x = MAX(0, r.x - b);
            int x2 = MIN(img.cols, r.x + r.width + b);
            int y = MAX(0, r.y - b);
            int y2 = MIN(img.rows, r.y + r.height + b);

            return img(Rect(x, y, x2 - x, y2 - y));
//            rectangle(img, uic->getBox().tl() - Point(4, 4), uic->getBox().br() + Point(4, 4), Scalar(0, 0, 255), 3);
//            return img;
        }

        void UicDebugHelper::drawWord(Mat &img, const PTextLine &line) {
//            rectangle(img, line->getBoundingBox().tl(), line->getBoundingBox().br(), Scalar(255, 0, 0), 2);
            if ((*line->getCharacters())[0]->getPrediction() != 0) {
                for (const Ptr<CharacterCandidate> &ch:(*line->getCharacters())) {
                    string text(1, ch->getPrediction());
                    drawText(text, ch->center, ch->height,
                             img);
                }
            } else {
                drawText(line->getPrediction(), getCenter(line->rect), line->rect.height * 8 / 10,
                         img);
            }
        }

        Mat UicDebugHelper::drawTextPredictions(const PLineVector &lines,
                                                const Mat &original) {
            Mat img;
            original.copyTo(img);
            for (auto &line:*lines) {
                drawWord(img, line);
            }
            return img;
        }

        Mat UicDebugHelper::drawLines(const PLineVector &lines,
                                      const Size &outSize) {
            Mat tmp = Mat::zeros(outSize.height, outSize.width, CV_WINDOW_NORMAL);
            for (const PTextLine &line:*lines) {
                drawLine(tmp, line);
            }
            return tmp;
        }

        void UicDebugHelper::drawLine(const Mat &out, const PTextLine &line) {
            for (const Ptr<CharacterCandidate> &ch:(*line->getCharacters())) {
                ch->draw(out);
            }
        }

        Mat UicDebugHelper::drawSeperateLines(PLineVector lines,
                                              const Size &outSize) {
            std::sort(lines->begin(), lines->end(),
                      [](const PTextLine &a, const PTextLine &b) -> bool {
                          return a->size() > b->size();
                      });

            Mat tmp = Mat::zeros(outSize.height, outSize.width, CV_WINDOW_NORMAL);
            int maxW = 0;
            int height = 0;
            int n = 0;

            for (PTextLine &line:*lines) {
                maxW = MAX(maxW, line->getBoundingBox().width);
                height += line->getBoundingBox().height;
                if (n++ > 5)
                    break;
            }
            Mat out = Mat::zeros(height, maxW, CV_8U);
            height = 0;
            n = 0;
            for (PTextLine &line:*lines) {
                Rect lineRect = line->getBoundingBox();
                tmp(line->getBoundingBox()) = Scalar(0);
                for (const Ptr<CharacterCandidate> &ch:(*line->getCharacters())) {
                    ch->draw(tmp);
                }
                Mat to = out(Rect(0, height, line->getBoundingBox().width, line->getBoundingBox().height));
                tmp(line->getBoundingBox()).copyTo(to);
                height += line->getBoundingBox().height;
                if (n++ > 5)
                    break;
            }
            bitwise_not(out, out);
            return out;
        }

        Mat UicDebugHelper::drawRandomChars(PCharVector chars, int MAX_ROS, int MAX_COLUMNS) {

            auto engine = std::default_random_engine(0);
            std::shuffle(std::begin(*chars), std::end(*chars), engine);

            int n = (int) chars->size();
            int SIZE = 32;
            int SPACING = SIZE / 4;
            int cols = MIN(n, MAX_COLUMNS);
            int rows = MIN((n + MAX_COLUMNS) / MAX_COLUMNS, MAX_ROS);
            int w = MAX(0, cols * SIZE + (cols - 1) * SPACING);
            int h = MAX(0, rows * SIZE + (rows - 1) * SPACING);
            int step = SIZE + SPACING;
            Mat out = Mat::zeros(h, w, CV_8UC1);
            int x = 0, y = 0;
            for (PCharacterCandidate ch: *chars) {
                if (x + SIZE > w) {
                    x = 0;
                    y += step;
                    if (y + SIZE > h)
                        break;
                }
                Rect to(x, y, SIZE, SIZE);
                Mat img = resizeWithBorder(ch->getImage(), SIZE, SIZE);
                img.copyTo(out(to));
                x += step;
            }
            bitwise_not(out, out);
            return out;
        }

        Mat UicDebugHelper::drawRandomCharsWithPrediction(PCharVector chars) {

            PCharVector withPrediction = new CharVector();
            for (auto &ch: *chars) {
                if (ch->getPrediction() > 0)
                    withPrediction->push_back(ch);
                else
                    cout << "Nothing" << endl;
            }
            auto engine = std::default_random_engine(0);
            std::shuffle(std::begin(*withPrediction), std::end(*withPrediction), engine);

            int n = (int) withPrediction->size();
            int MAX_COLUMNS = 10;
            int MAX_ROS = 10;
            int SIZE = 32;
            int SPACING = SIZE / 4;
            int cols = MIN(n, MAX_COLUMNS);
            int rows = MIN((n + MAX_COLUMNS) / MAX_COLUMNS, MAX_ROS);
            int w = MAX(0, cols * SIZE * 2 + (cols - 1) * (SPACING));
            int h = MAX(0, rows * SIZE + (rows - 1) * SPACING);
            int step = SIZE + SPACING;
            Mat out = Mat::zeros(h, w, CV_8UC1);
            int x = 0, y = 0;

            for (PCharacterCandidate ch: *withPrediction) {
                if (x + SIZE > w) {
                    x = 0;
                    y += step;
                    if (y + SIZE > h) {
                        break;
                    }
                }
                Rect to(x, y, SIZE, SIZE);
                Mat img = resizeWithBorder(ch->getImage(), SIZE, SIZE);
                img.copyTo(out(to));


                string text(1, ch->getPrediction());
                if (ch->getConfidence() >= 0) {
                    drawText(text, Point(x + SIZE * 4 / 3, y + SIZE / 2), SIZE * 2 / 3,
                             out, Scalar(128));
                } else {
                    drawText(text, Point(x + SIZE * 4 / 3, y + SIZE / 2), SIZE,
                             out, Scalar(255));
                }

                x += step + SIZE;
            }
            bitwise_not(out, out);
            return out;
        }

        Mat UicDebugHelper::drawSeperateChars(const PLineVector &lines,
                                              const Size &outSize) {
            int sumHeight = 0;
            int maxWidth = 0;
            for (const PTextLine &line:*lines) {
                int sumWidth = 0;
                int maxHeight = 0;
                for (const PCharacterCandidate &ch: *line->getCharacters()) {
                    sumWidth += ch->width;
                    maxHeight = MAX(maxHeight, ch->height);
                }
                cout << endl;
                sumHeight += maxHeight;
                maxWidth = MAX(maxWidth, sumWidth);
            }
            maxWidth *= 2;
            Mat img = Mat::zeros(sumHeight, maxWidth, CV_8UC1);
            int y = 0;
            for (const PTextLine &line:*lines) {
                int x = 0;
                int maxHeight = 0;
                for (const PCharacterCandidate &ch: *line->getCharacters()) {
                    Mat chImg = ch->getImage();
                    Rect rect(x + ch->width / 2 - chImg.cols / 2, y + ch->height / 2 - chImg.rows / 2, chImg.cols,
                              chImg.rows);
                    chImg.copyTo(img(rect));
                    maxHeight = MAX(maxHeight, ch->height);
                    x += ch->width * 2;
                }
                y += maxHeight;
            }
            return img;
        }


        Mat UicDebugHelper::drawSeperateLineOutlines(const vector<TextLine> &lines,
                                                     const Size &outSize) {
            Mat tmp = Mat::zeros(outSize.height, outSize.width, CV_8UC1);
            int maxW = 0;
            int height = 0;
            for (const TextLine &line:lines) {
                maxW = MAX(maxW, line.getBoundingBox().width);
                height += line.getBoundingBox().height;
            }
            Mat out = Mat::zeros(height, maxW, CV_8U);
            height = 0;
            for (const TextLine &line:lines) {
                Rect lineRect = line.getBoundingBox();
                tmp(line.getBoundingBox()) = Scalar(0);
                int h = lineRect.height / (int) line.getCharacters()->size();
                int i = 0;
                for (const Ptr<CharacterCandidate> &ch:(*line.getCharacters())) {
                    Rect r(ch->getLeft(), lineRect.y + h * i, ch->width, h);
                    i++;
                    rectangle(tmp, r, Scalar(0xFF), -1);
                    int x = ch->getLeft();
                    cv::line(tmp, Point(x, lineRect.y),
                             Point(x, lineRect.y + lineRect.height), Scalar(128), 1);
                    x = ch->getRight();
                    cv::line(tmp, Point(x, lineRect.y),
                             Point(x, lineRect.y + lineRect.height), Scalar(128), 1);
                }
                Mat to = out(Rect(0, height, line.getBoundingBox().width, line.getBoundingBox().height));
                tmp(line.getBoundingBox()).copyTo(to);
                height += line.getBoundingBox().height;
            }
            return out;
        }

        Mat UicDebugHelper::drawChars(const PCharVector &characters,
                                      const Size &outSize) {
            Mat out = Mat::zeros(outSize.height, outSize.width, CV_WINDOW_NORMAL);
            for (auto it = characters->rbegin(); it < characters->rend(); it++) {
                (*it)->draw(out);
            }
            return out;
        }

        Mat UicDebugHelper::drawCharacterCandidates(const vector<Ptr<CharacterCandidate>> &characters,
                                                    const Mat &original) {
            Mat img;
            original.copyTo(img);
            for (auto &ch:characters) {
                rectangle(img, ch->getRect().tl(), ch->getRect().br(), Scalar(0, 255, 255), 2);
            }
            return img;
        }

        void
        UicDebugHelper::drawText(const string &text, const Point &center, int height, Mat &img, const Scalar &color) {
            Size word_size = getTextSize(text, FONT_HERSHEY_SIMPLEX, 1, 1,
                                         NULL);
            double scale = height / (double) word_size.height * 0.8;

            Point pos((int) (center.x - word_size.width * scale / 2),
                      (int) (center.y + word_size.height * scale / 2));

            putText(img, text, pos, FONT_HERSHEY_SIMPLEX, scale,
                    Scalar(0, 0, 0), (int) (scale * 4));
            putText(img, text, pos, FONT_HERSHEY_SIMPLEX, scale,
                    color, (int) (scale * 2));
        }

        void UicDebugHelper::show(const string &name, const Mat &img) {
            if (img.cols > 0 && img.rows > 0) {
                namedWindow(name, CV_WINDOW_NORMAL);
                imshow(name, img);
            } else {
                cout << "Could not show image \"" << name << "\" (no valid dimensions)" << endl;
            }
        }

        void UicDebugHelper::saveChars(const PCharVector &chars, string path) {
            int index = 0;
            for (auto ch: *chars) {
                if (ch->getPrediction() >= '0' && ch->getPrediction() <= '9') {
                    Mat img = resizeWithBorder(ch->getImage(), 58, 58);
                    std::stringstream ss;

                    int prediction = ch->getPrediction() - '0';

                    ss << path << (48 + prediction) << "_" << (30 + prediction) << "_" << index << ".png";
                    imwrite(ss.str(), img);
                    index++;
                }
            }

        }


        void UicDebugHelper::save(Mat img, string name) {
            if (imagePath.length() > 0) {
                std::stringstream ss;
                if (name.length() > 0) {
                    ss << imagePath << name << ".png";
                } else {
                    ss << imagePath << imageIndex << ".png";
                }
                imwrite(ss.str(), img);
                imageIndex++;
            }
        }

        void UicDebugHelper::printTimings(PUicProcess uicDetector) {
            TimeLogger::printAll();
//            Ptr<ParentImageProcess> cast = uicDetector.dynamicCast<ParentImageProcess>();
//            while (!cast.empty()) {
//                TimeLogger::print(cast->toString());
//                cast = cast->getChild().dynamicCast<ParentImageProcess>();
//            }
//            TimeLogger::print(uicDetector->getChildPipeline()->getChildPipeline()->getChildPipeline()->getProcessName());
//            TimeLogger::print(uicDetector->getChildPipeline()->getChildPipeline()->getProcessName());
//            TimeLogger::print(uicDetector->getChildPipeline()->getProcessName());
        }

        Mat UicDebugHelper::drawDigits(Mat img, UicEntry uic, UicEntry::ImageType mode) {
            Mat result;
            img.copyTo(result);
            if (uic.digitRects.empty()) {
                return result;
            }
            int stroke = MAX(1, uic.getMinDigitHeight() / 15);
            for (auto digit:uic.digitRects) {
//                digit -= uic.rectUic.tl();
                digit = uic.translate(digit, mode);
                rectangle(result, digit.tl(), digit.br(), Scalar(255, 0, 0), stroke);
            }
            return result;
        }
    }
}