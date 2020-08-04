//
// Created by fab on 07.03.17.
//

#ifndef UIC_CHARSCALEPIEPLINE_H
#define UIC_CHARSCALEPIEPLINE_H

#include "ScaleWrapper.h"
#include "CharacterCandidate.h"
#include "CharacterDetector.h"
#include "ScaledCharacter.h"
#include "CharGridFilter.h"

namespace cv {
    namespace text {
        class GridFilterProcess : public WrapperImageProcess<PCharVector> {
            int n;
        public:
            GridFilterProcess(const PCharProcess &delegate, int n) {
                this->setChild(delegate);
                this->n = n;
            }

            inline PCharVector processImage(Mat original,Mat img){
                CharGridFilter filter(Size(img.cols, img.rows));
                const PCharVector &in = child->processImage(original, img);
                cout<<"In: "<<in->size()<<endl;
                const PCharVector &result = filter.filter(in, n);
                cout<<" Out: "<<result->size()<<endl;
                return result;
            }
            string getProcessName(){
                return child->toString() + " > GridFilterProcess";
            }

        };
    }
}
#endif //UIC_CHARSCALEPIEPLINE_H
