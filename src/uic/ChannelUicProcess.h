//
// Created by fab on 31.03.17.
//

#ifndef UIC_CHANNELUICPROCESS_H
#define UIC_CHANNELUICPROCESS_H

#include <ChannelWrapper.h>
#include "UicDetector.h"

namespace cv {
    namespace uic {
        class ChannelUicProcess : public ChannelWrapper<PUicResult> {
        public:
            ChannelUicProcess(PUicProcess delegate, int channels = ImageUtils::CHANNELS_LUMINANCE)
            : ChannelWrapper(delegate,channels) {
            }
            virtual PUicResultVector processImage(Mat original, Mat img) {
                PUicResultVector out = ChannelWrapper::processImage(original,img);
                std::sort(out->begin(), out->end(), UicDetector::sortByScore);
                return out;
            }
        };
    }
}
#endif //UIC_CHANNELUICPROCESS_H
