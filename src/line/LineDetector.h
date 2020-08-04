//
// Created by fab on 15.11.16.
//

#ifndef UIC_LINEDETECTOR_H
#define UIC_LINEDETECTOR_H

#include "TextLine.h"
#include "../char/CharacterDetector.h"
#include "../utils/TimeLogger.h"

namespace cv {
    namespace text {
        typedef ImageProcess<PLineVector> LineProcess;
        typedef Ptr<LineProcess> PLineProcess;
        class LineDetector : public ImagePipeline<PCharVector, PLineVector> {
        public:
            LineDetector(PCharProcess charDetector) {
                setChild(charDetector);
            }

//            virtual vector<TextLine> findLines(PCharVector chars)=0;
            virtual PLineVector process(Mat &original,Mat &in,PCharVector chars)=0;
        };
        typedef Ptr<LineDetector> PLineDetector;

    }
}


#endif //UIC_LINEDETECTOR_H
