//
// Created by fab on 12.08.17.
//

#include "CharGridFilter.h"


namespace cv {
    namespace text {
        unsigned int CharGridFilter::getScale(int size) {
            return (unsigned int)log2(size);
        }
    }
}