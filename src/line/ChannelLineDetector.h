//
// Created by fab on 05.03.17.
//

#ifndef UIC_CHANNELLINEDETECTOR_H
#define UIC_CHANNELLINEDETECTOR_H

#include "LineDetector.h"

namespace cv {
    namespace text {
//        class ChannelLineDetector : public LineDetector {
//        private:
//            PLineDetector detector;
//            int mode;
//        public:
//            inline ChannelLineDetector(PLineDetector lineDetector,
//                                            int mode = ImageUtils::CHANNELS_GRAY):LineDetector(lineDetector->getChild().dynamicCast<CharacterDetector>()) {
//                this->detector = lineDetector;
//                this->mode = mode;
//            }
//
//            inline PLineVector processImage(Mat original,Mat img){
//                vector <Mat> channels;
//                ImageUtils::splitInChannels(img, channels, this->mode);
//
////                Mat lab;
////                cvtColor(img, lab, COLOR_RGB2Lab);
////                detector->getChild()->setOriginal(lab);
//
//                PLineVector lines = new vector<TextLine>();
//                for (int c = 0; c < (int) channels.size(); c++) {
//                    PLineVector newLines = LineDetector::processImage(original,channels[c]);
//                    lines->insert(std::end(*lines), std::begin(*newLines), std::end(*newLines));
//                }
//                return lines;
//            }
//
//            virtual PLineVector process(Mat &original,Mat &in,PCharVector chars){
//                return detector->process(original,in,chars);
//            }
//
//            inline string getName() {
//                return "ChannelLineDetector(" + detector->toString() + ")";
//            }
//        };

    }
}
#endif //UIC_CHANNELLINEDETECTOR_H
