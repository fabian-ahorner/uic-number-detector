//
// Created by fab on 09.12.16.
//

#ifndef UIC_STORAGEPIPELINE_H
#define UIC_STORAGEPIPELINE_H

#include "UicDetector.h"
#include "../ui/UicSelector.h"

namespace cv {
    namespace uic {
        class UicDebugHelper {
        public:

            static PUicResultVector computeAndShowIntermediates(Mat img, PUicProcess uicDetector);
            static void printTimings(PUicProcess uicDetector);
            static int imageIndex;
            static string imagePath;

            static void drawWord(Mat &img, const PTextLine &line);

            static void drawText(const string &text, const Point &center, int height, Mat &img, const Scalar& color = Scalar(255,255,255));

            static void showWords(const Mat &image, const PLineVector lines);

            static Mat drawUics(const Mat &image, const PUicResult& uics);
            static Mat drawUicResult(const Mat &image, const PUicResult& uics);

            static Mat drawTextPredictions(const PLineVector &lines,
                                           const Mat &original);

            static Mat drawLines(const PLineVector &lines,
                                 const Size &outSize);

            static Mat drawSeperateLines(PLineVector lines,
                                         const Size &outSize);

            static Mat drawSeperateLineOutlines(const vector<TextLine> &lines,
                                                const Size &outSize);

            static Mat drawSeperateChars(const PLineVector &lines,
                                         const Size &outSize);

            static Mat drawChars(const PCharVector &characters,
                                 const Size &outSize);

            static void show(const string &name,
                             const Mat &img);

            static void save(Mat img, string basicString="");
            static void saveChars(const PCharVector& chars, string image);

            static Mat drawCharacterCandidates(const vector<Ptr<CharacterCandidate>> &characters,
                                               const Mat &original);

            static void drawLine(const Mat &out, const PTextLine &line);

            static Mat drawUicShape(Mat &image, const PUicResult& uics);

            static Mat drawDigits(Mat img, UicEntry uic, UicEntry::ImageType type);

            static Mat drawRandomChars(PCharVector chars, int rows=5, int cols=10);
            static Mat drawRandomCharsWithPrediction(PCharVector chars);
        };
    }
}


#endif //UIC_STORAGEPIPELINE_H
