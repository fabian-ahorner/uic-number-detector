//
// Created by fab on 19.02.17.
//

#ifndef UIC_CHANNELCHARACTERDETECTOR_H
#define UIC_CHANNELCHARACTERDETECTOR_H

namespace cv {
    namespace text {
        class ChannelCharacterDetector : public CharacterDetector {
        private:
            PCharProcess detector;
            int mode;
        public:
            inline ChannelCharacterDetector(PCharProcess singleChannelDetector,
                                            int mode = ImageUtils::CHANNELS_LUMINANCE) {
                this->detector = singleChannelDetector;
                this->mode = mode;
            }

            inline PCharVector processImage(Mat original,Mat img){



                vector <Mat> channels;
                ImageUtils::splitInChannels(img, channels, this->mode);

                Mat lab;
                cvtColor(img, lab, COLOR_RGB2Lab);
                detector->setOriginal(lab);
//                detector->setStartId(getCurrentId());


                PCharVector characters = new vector<PCharacterCandidate>();
                for (int c = 0; c < (int) channels.size(); c++) {
                    PCharVector newCharacters = detector->processImage(original,channels[c]);
                    characters->insert(std::end(*characters), std::begin(*newCharacters), std::end(*newCharacters));
                }
                return characters;
            }

            inline string getProcessName() {
                return "ChannelCharacterDetector(" + detector->toString() + ")";
            }
        };

    }
}


#endif //UIC_CHANNELCHARACTERDETECTOR_H
