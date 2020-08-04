//
// Created by fab on 07.03.17.
//

#ifndef UIC_CHANNELPIPELINE_H
#define UIC_CHANNELPIPELINE_H

#include "ImagePipeline.h"
#include "ImageUtils.h"
#include "../utils/DataCollector.h"

namespace cv {
    template<class OUT>
    class ChannelWrapper : public WrapperImageProcess<Ptr<vector<OUT>>> {
        const int channels;

    public:
        ChannelWrapper(Ptr<ImageProcess<Ptr<vector<OUT>>>> delegate, int channels = ImageUtils::CHANNELS_LUMINANCE)
                : channels(channels) {
            this->setChild(delegate);
        }

        virtual Ptr<vector<OUT>> processImage(Mat original,Mat img) {
            DataCollector<OUT> collector;
            vector<Mat> channels;
            ImageUtils::splitInChannels(img, channels, this->channels);
            for (int c = 0; c < (int) channels.size(); c++) {
                collector.add(this->child->processImage(original,channels[c]));
            }
            return collector.dataPtr();
        }

        inline string getProcessName() {
            return this->child->toString() + " >> Channels";
        }
    };

    template<class OUT>
    class GrayScaleWrapper : public WrapperImageProcess<Ptr<vector<OUT>>> {

    public:
        GrayScaleWrapper(Ptr<ImageProcess<Ptr<vector<OUT>>>> delegate) {
            this->setChild(delegate);
        }

        virtual Ptr<vector<OUT>> processImage(Mat original,Mat img) {
            Mat originalGray;
            Mat imgGray;
            cvtColor(original, originalGray, COLOR_BGR2GRAY);
            cvtColor(img, imgGray, COLOR_BGR2GRAY);

            return this->child->processImage(originalGray,imgGray);
        }

        inline string getProcessName() {
            return this->child->toString() + " >> Gray";
        }
    };
}


#endif //UIC_CHANNELPIPELINE_H
