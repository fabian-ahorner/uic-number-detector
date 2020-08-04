//
// Created by fab on 12.11.16.
//

#include <opencv2/opencv.hpp>
#include "TextDetector.h"
#include "CharClassifier.h"

namespace cv {
    namespace text {
        bool TextDetector::isRepetitive(const string &s) {
            int count = 0;
            for (int i = 0; i < (int) s.size(); i++) {
                if ((s[i] == 'i') ||
                    (s[i] == 'l') ||
                    (s[i] == 'I'))
                    count++;
            }
            return count > ((int) s.size() + 1) / 2;
        }

        PLineVector TextDetector::process(Mat &original, Mat &img, PLineVector lines) {
            for (auto &line:*lines) {
                if (!line->hasPrediction()) {
                    classifyLine(img, line);
                }
            }
            return lines;
        }

        void SingleCharClassifier::classifyLine(Mat &img, const PTextLine &line) {
            string text;
            vector<PCharacterCandidate> &l = *line->getCharacters();
            for (Ptr<CharacterCandidate> ch:l) {
//                Mat charImg = ch->getImage();
                char prediction = classifyChar(img, ch);
//                ch->setPrediction(prediction);
                text += string(1, prediction);
            }
            line->setPrediction(text);
        }

        void LineClassifier::classifyLine(Mat &img, const PTextLine &line) {
            Mat lineImg = line->drawLine(Size(img.cols, img.rows));
            string prediction = classifyLine(img, lineImg);
            line->setPrediction(prediction);
        }

        char TesseractSingleCharClassifier::classifyChar(Mat &original, Ptr<CharacterCandidate> &ch) {

            Mat img = ch->getImage();
            vector<Rect> boxes;
            vector<string> words;
            vector<float> confidences;
            string output;
            copyMakeBorder(img, img, 15, 15, 15, 15, BORDER_CONSTANT, Scalar(0));
            ocr->run(img, output, &boxes, &words, &confidences, OCR_LEVEL_WORD);
            float maxConfidence = 0;
            int bestChar = -1;
            for (int j = 0; j < (int) boxes.size(); j++) {
                if (confidences[j] > maxConfidence) {
                    maxConfidence = confidences[j];
                    bestChar = j;
                }
            }
            char prediction = bestChar == -1 ? '*' : words[bestChar][0];
            if (prediction < '0' || prediction > '9') {
                prediction = '*';
            }
            ch->setPrediction(prediction, maxConfidence);
            return prediction;
        }

        void TesseractSingleCharClassifier::classifyLine(Mat &img, const PTextLine &line) {
            ocr->clearAdaptiveClassifier();
            SingleCharClassifier::classifyLine(img, line);
        }

//        PLineVector TesseractSingleCharClassifier::process(Mat &original,Mat &img, PLineVector lines) {
//            return SingleCharClassifier::process(original,img, lines);
//        }


        char SVMSingleCharClassifier::classifyChar(Mat &original, Ptr<CharacterCandidate> &ch) {
            Mat img = ch->getImage();
            Mat features = resizeWithBorder(img, CharClassifier::CHAR_CLASSIFIER_SIZE,
                                            CharClassifier::CHAR_CLASSIFIER_SIZE);
            features = features.reshape(1, 1);
            features.convertTo(features, CV_32F);

            float prediction = charClassifier->predict(features);

            char result = prediction < 10 ? '0' + (char) prediction : '*';
            ch->setPrediction(result);
            return result;
        }

        char ComboOCR::classifyChar(Mat &original, Ptr<CharacterCandidate> &ch) {
            char result = tess->classifyChar(original, ch);
            if (result < '0' || result > '9') {
                result = svm->classifyChar(original, ch);
                ch->setPrediction(result, 1);
            }
            return result;
        }

        string TesseractLineCharClassifier::classifyLine(Mat &original, Mat &line) {
            vector<Rect> boxes;
            vector<string> words;
            vector<float> confidences;
            string output;
            copyMakeBorder(line, line, 15, 15, 15, 15, BORDER_CONSTANT, Scalar(0));
            ocr->run(line, output, &boxes, &words, &confidences, OCR_LEVEL_WORD);
            string text;
            for (int j = 0; j < (int) boxes.size(); j++) {
//                if ((words[j].size() < 2) || (confidences[j] < 51) ||
//                    ((words[j].size() == 2) && (words[j][0] == words[j][1])) ||
//                    ((words[j].size() < 4) && (confidences[j] < 60)) ||
//                    TextDetector::isRepetitive(words[j]))
//                    continue;
                text += words[j];
            }

            return text;
        }

        void TesseractLineCharClassifier::classifyLine(Mat &img, const PTextLine &line) {
            ocr->clearAdaptiveClassifier();
            LineClassifier::classifyLine(img, line);
        }
    }
}
