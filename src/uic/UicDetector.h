//
// Created by fab on 18.11.16.
//

#ifndef UIC_UICDETECTOR_H
#define UIC_UICDETECTOR_H

#include "../line/TextLine.h"
#include "../ocr/TextDetector.h"

namespace cv {
    namespace uic {
        using namespace text;

        struct LineStats {
            int availableCharacters = 0;
            int offset = 0;
            int validCharacters = 0;
            int nonDigits = 0;
            double digitConfidence = 0;

            double getScore(double weight) {
                return validCharacters + weight * digitConfidence;
            }
        };

        class UicResult {
            double score;

            double getConfidence(const PTextLine &line, int length);

        public:
            UicResult();

//            int missingChars;
            string uic;
            PTextLine lines[3];
            LineStats lineStats[3];

            int getLineOffset(int leftMostX, const PTextLine &line);

            UicResult(const PTextLine &line1, const PTextLine &line2, const PTextLine &line3);

            bool isValid() const;

            inline Rect getBox() const {
                return lines[0]->rect | lines[1]->rect | lines[2]->rect;
            }

            inline double getScore() const {
                return score ;
            }

            int countNonDigits(const string &str, int i);

            inline bool operator<(UicResult &other) {
                return getScore() > other.getScore();
            }

            bool isDigit(char character);

            void update();

            cv::uic::LineStats calculateStats(int leftMostX, const PTextLine &line, int maxLength);

            Mat preprocessLine(int line,Mat img);
        };

        typedef Ptr<UicResult> PUicResult;
        typedef vector<PUicResult> UicResultVector;
        typedef Ptr<UicResultVector> PUicResultVector;
        typedef Ptr<ImageProcess<PUicResultVector>> PUicProcess;

        class UicDetector : public ImagePipeline<PLineVector, PUicResultVector> {

        public:
            virtual PUicResultVector process(Mat &original, Mat &in, PLineVector lines)=0;

            UicDetector(PLineProcess lineDetector) {
                setChild(lineDetector);
            }
            static bool sortByScore(PUicResult &a, PUicResult &b) {
                return *a < *b;
            }
        };


        class SimpleUicDetector : public UicDetector {
        private:
            static Rect getSearchArea(const PTextLine &line);

            static void updateSearchArea(Rect &searchArea, const PTextLine &line);

            PUicResult extractUICFromLines(Mat &original, LineVector &vector);

            float maxHeight;
            float minHeight;
            bool skipOCR = true;
            Ptr<TextDetector> textDetector;

        public:
            inline SimpleUicDetector(PLineProcess lineDetector, Ptr<TextDetector> textDetector) : UicDetector(
                    lineDetector), textDetector(textDetector) {}


            PUicResultVector process(Mat &original, Mat &in, PLineVector lines);

            void extractUics(Mat &original, PLineVector &lines, Rect searchArea, int iNextLine,
                             LineVector matchingLines, PUicResultVector &uics);

            inline string getProcessName() {
                return "SimpleUicDetector";
            }

            PUicResultVector filter(PUicResultVector &uics);

            virtual inline PUicResultVector processImage(Mat original, Mat img) {
                if (skipOCR) {
                    return UicDetector::processImage(original, img);
                } else {
                    PLineVector in = this->getChild()->processImage(original, img);
                    return processStep(original, img, textDetector->processStep(original, img, in));;
                }
            }

            void addResult(PUicResultVector &uic, PUicResult ptr);

            void doOCR(Mat &original, PUicResultVector &uics);

            void combine(PUicResultVector uics);

            void combine(PTextLine &line, PTextLine &other);
        };

        class ChannelUicDetector : public SimpleUicDetector {
            const int mode;
        public:
            PUicResultVector processImage(Mat original, Mat img);

            inline ChannelUicDetector(PLineProcess lineDetector, Ptr<TextDetector> textDetector,
                                      int mode = ImageUtils::CHANNELS_LUMINANCE)
                    : SimpleUicDetector(lineDetector, textDetector), mode(mode) {}

            inline string getProcessName() {
                return "ChannelUicDetector";
            }
        };
    }
}


#endif //UIC_UICDETECTOR_H
