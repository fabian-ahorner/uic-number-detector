//
// Created by fab on 07.03.17.
//

#ifndef UIC_SCALEPIPELINE_H
#define UIC_SCALEPIPELINE_H

#include "ImagePipeline.h"
#include "ImageUtils.h"
#include "../utils/DataCollector.h"

namespace cv {
    template<class OUT>
    class ScaleWrapper : public WrapperImageProcess<Ptr<vector<OUT>>> {
        typedef Ptr<vector<OUT>> OutVector;
        typedef Ptr<ImageProcess<OutVector>> DelegateProcess;

        const double scale;
        const Size minSize;
    public:

        virtual OutVector scaleOut(const OutVector &inVector, const Size &currentSize, const Size &originalSize) {
            OutVector out = new vector<OUT>();
            for (OUT &in : *inVector) {
                out->push_back(scaleOut(in, currentSize, originalSize));
            }
            return out;
        }

        virtual OUT scaleOut(const OUT &in, const Size &currentSize, const Size &originalSize)=0;

        ScaleWrapper(const DelegateProcess &delegate, double scale = 1.6, const Size &minSize = Size(1, 1))
                : scale(scale), minSize(minSize) {
            this->setChild(delegate);
        }

        virtual OutVector processImage(Mat original, Mat img) {
            DataCollector<OUT> collector;
            double currentScale = 1;
            Size originalSize(img.cols, img.rows);
            Size currentSize = originalSize;
            while (img.cols > minSize.width && img.rows > minSize.height) {
                OutVector scaled = scaleOut(this->child->processImage(original, img), currentSize, originalSize);

                collector.add(scaled);

                currentScale /= this->scale;
                currentSize = Size(cvRound(originalSize.width * currentScale),
                                   cvRound(originalSize.height * currentScale));
                resize(img, img, currentSize);
                resize(original, original, currentSize);
            }
            return collector.dataPtr();
        }

        inline string getProcessName() {
            return this->child->toString() + " >> Scale Pyramid";
        }
    };
}


#endif //UIC_SCALEPIPELINE_H
