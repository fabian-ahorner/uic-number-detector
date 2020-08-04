//
// Created by fab on 11.01.17.
//

#ifndef UIC_UICEVALUATOR_H
#define UIC_UICEVALUATOR_H


#include "UicDataset.h"
#include "UicDetector.h"
#include "UicDebugHelper.h"

namespace cv {
    namespace uic {
        class UicEvaluator {
        public:
            static void
            evaluateUicStrings(PUicProcess detector, const UicDataset &dataset, UicEntry::ImageType mode,
                               const vector<string> whitelist = vector<string>());
            static void evaluateCharDetector(PCharProcess detector, const UicDataset &dataset,
                                                    UicEntry::ImageType mode);
            static void evaluateResult(const UicEntry &original, const PUicResultVector &uicResults,
                                       UicEntry::ImageType mode);
        private:
            static int countMatches(string uic, vector<TextLine> lines);

            static float getOverlap(const Rect &r1, const Rect &r2);

            static vector<Rect> getRects(PCharVector chars);

            static vector<Rect> getTargetRects(const UicEntry &uic, UicEntry::ImageType type);

            static double getScore(vector<Rect> rects1, vector<Rect> rects2);

            static double getMaxOverlap(Rect rect, vector<Rect> rects);
        };
    }
};


#endif //UIC_UICEVALUATOR_H
