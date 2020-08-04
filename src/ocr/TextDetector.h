//
// Created by fab on 12.11.16.
//

#ifndef UIC_TEXTDETECTOR_H
#define UIC_TEXTDETECTOR_H

#include "../line/TextLine.h"
#include "opencv2/ml.hpp"
#include "../line/LineDetector.h"
#include "OCRTesseract.h"

namespace cv {
    namespace text {
        using namespace cv::ml;

        class TextDetector : public ImagePipeline<PLineVector, PLineVector> {
        public:
            static bool isRepetitive(const string &s);

            virtual PLineVector process(Mat &original, Mat &img, PLineVector lines);

            inline TextDetector(PLineProcess lineDetector) {
                setChild(lineDetector);
            }

            inline TextDetector() {
            }

            virtual void classifyLine(Mat &img, const PTextLine &line)=0;
        };

//        class LineTextDetector : public TextDetector {
//        public:
//
//
//
//            virtual void classifyLine(Mat &img, TextLine &line)=0;
//        };

        class SingleCharClassifier : public TextDetector {
        public:
            inline SingleCharClassifier(PLineProcess lineDetector) : TextDetector(lineDetector) {}

            inline SingleCharClassifier() : TextDetector() {}

            virtual void classifyLine(Mat &img, const PTextLine &line);

            virtual char classifyChar(Mat &img, Ptr<CharacterCandidate> &character)=0;
        };

        class LineClassifier : public TextDetector {
        public:
            inline LineClassifier(PLineProcess lineDetector)
                    : TextDetector(lineDetector) {
            }

            inline LineClassifier()
                    : TextDetector() {
            }

            virtual void classifyLine(Mat &img, const PTextLine &line);

            virtual string classifyLine(Mat &img, Mat &line)=0;
        };


        class TesseractSingleCharClassifier : public SingleCharClassifier {
        private:
            const Ptr<OCRTesseract2> ocr;
        public:
            inline TesseractSingleCharClassifier(PLineProcess lineDetector, bool numbersOnly = true)
                    : SingleCharClassifier(lineDetector),
                      ocr(OCRTesseract2::create(NULL, NULL, numbersOnly ? "0123456789" : NULL, 2, 10)) {
//                ocr->setVariable("segment_penalty_garbage", "0");
//                ocr->setVariable("segment_penalty_dict_nonword", "0");
//                ocr->setVariable("segment_penalty_dict_frequent_word", "0");
//                ocr->setVariable("segment_penalty_dict_case_ok", "0");
//                ocr->setVariable("segment_penalty_dict_case_bad", "0");
//                ocr->setVariable("tessedit_enable_doc_dict", "0");
                ocr->disableAdaptiveClassifier();
            }

            inline TesseractSingleCharClassifier(bool numbersOnly = true)
                    : SingleCharClassifier(),
                      ocr(OCRTesseract2::create(NULL, NULL, numbersOnly ? "0123456789" : NULL, 2, 10)) {
//                ocr->setVariable("tessedit_enable_doc_dict", "0");
//                                ocr->setVariable("segment_penalty_garbage", "0");
//                ocr->setVariable("segment_penalty_dict_nonword", "0");
//                ocr->setVariable("segment_penalty_dict_frequent_word", "0");
//                ocr->setVariable("segment_penalty_dict_case_ok", "0");
//                ocr->setVariable("segment_penalty_dict_case_bad", "0");

                ocr->disableAdaptiveClassifier();
            }

            void classifyLine(Mat &img, const PTextLine &line);
//            PLineVector process(Mat& original,Mat &img, PLineVector lines);

            char classifyChar(Mat &img, Ptr<CharacterCandidate> &ch);

            inline void clearAdaptiveClassifier() {
                ocr->clearAdaptiveClassifier();
            }

            inline string getProcessName() {
                return "TesseractSingleCharClassifier";
            }
        };

        class TesseractLineCharClassifier : public LineClassifier {
        private:
            const Ptr<OCRTesseract2> ocr;
        public:
            inline TesseractLineCharClassifier(PLineProcess lineDetector, bool numbersOnly = false)
                    : LineClassifier(lineDetector),
                      ocr(OCRTesseract2::create(NULL, NULL, numbersOnly ? "0123456789" : NULL, 0, 3)) {
                if (numbersOnly)
                    ocr->setWhiteList("0123456789");
//                ocr->disableAdaptiveClassifier();
            }

            inline TesseractLineCharClassifier(bool numbersOnly = false)
                    : LineClassifier(),
                      ocr(OCRTesseract2::create(NULL, NULL, numbersOnly ? "0123456789" : NULL, 0, 3)) {
                if (numbersOnly)
                    ocr->setWhiteList("0123456789");
//                ocr->disableAdaptiveClassifier();
            }

            string classifyLine(Mat &img, Mat &line);

            inline string getProcessName() {
                return "TesseractLineCharClassifier";
            }

            void classifyLine(Mat &img, const PTextLine &line);
        };

        class SVMSingleCharClassifier : public SingleCharClassifier {
        private:
            Ptr<ml::StatModel> charClassifier;
            TesseractSingleCharClassifier tess;
        public:
            inline SVMSingleCharClassifier(PLineProcess lineDetector, string charClassifierPath)
                    : SingleCharClassifier(lineDetector) {
                charClassifier = StatModel::load<SVM>(charClassifierPath);
//                tess = new TesseractSingleCharClassifier
            }

            inline SVMSingleCharClassifier(string charClassifierPath="/home/fab/dev/cpp/uic/models/character_classifier_test.xml")
                    : SingleCharClassifier() {
                charClassifier = StatModel::load<SVM>(charClassifierPath);
            }

            char classifyChar(Mat &img, Ptr<CharacterCandidate> &ch);

            inline string getProcessName() {
                return "SVMSingleCharClassifier";
            }
        };


        class ComboOCR: public SingleCharClassifier {
            Ptr<TesseractSingleCharClassifier> tess;
            Ptr<SVMSingleCharClassifier> svm;
        public:

            inline ComboOCR(string path = "/home/fab/dev/cpp/uic/models/character_classifier_test.xml")
                    : SingleCharClassifier(),
                      tess(new TesseractSingleCharClassifier(true)), svm(new SVMSingleCharClassifier(path)) {
            }
            inline ComboOCR(PLineProcess lineDetector,string path = "/home/fab/dev/cpp/uic/models/character_classifier_test.xml")
                    : SingleCharClassifier(lineDetector),
                      tess(new TesseractSingleCharClassifier(true)), svm(new SVMSingleCharClassifier(path)) {
            }
            char classifyChar(Mat &img, Ptr<CharacterCandidate> &ch);

            inline string getProcessName() {
                return "ComboOCR";
            }
        };
    }
}


#endif //UIC_TEXTDETECTOR_H
