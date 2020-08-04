//
// Created by fab on 06.03.17.
//

#ifndef UIC_SCALEDCHARACTER_H
#define UIC_SCALEDCHARACTER_H

#include "CharacterCandidate.h"

namespace cv {
    namespace text {
        class ScaledCharacter : public CharacterCandidate {
            const PCharacterCandidate ch;

            Rect scaleBox(const Rect &scaledBox, const Size &scaledSize, const Size &originalSize) {
                int boxX = scaledBox.x * originalSize.width / scaledSize.width;
                int boxY = scaledBox.y * originalSize.height / scaledSize.height;
                int boxW = scaledBox.width * originalSize.width / scaledSize.width;
                int boxH = scaledBox.height * originalSize.height / scaledSize.height;
                return cv::Rect(boxX, boxY, boxW, boxH);
            }

        public :
            inline ScaledCharacter(PCharacterCandidate ch, Size scaledSize, Size originalSize)
                    : CharacterCandidate(scaleBox(ch->rect, scaledSize, originalSize), ch->id), ch(ch) {
            }

            Mat getImage() {
                return ch->getImage();
            }

            inline void draw(Mat img) {
                Mat charImg = getImage();
                resize(charImg, charImg, Size(rect.width, rect.height));
                charImg.copyTo(img(rect));
            }

            inline double getIsCharProbability() {
                return ch->getIsCharProbability();
            }

            inline double getCharQuality() {
                return ch->getCharQuality();
            }
            inline double getConfidence() {
                return ch->getConfidence();
            }

            inline void setPrediction(char prediction, double confidence) {
                ch->setPrediction(prediction,confidence);
                this->prediction = prediction;
                this->confidence = confidence;
            }
            inline char getPrediction() const {
                return ch->getPrediction();
            }
            inline void setPrediction(char prediction) {
                ch->setPrediction(prediction);
                this->prediction = prediction;
            }

            static inline PCharVector from(PCharVector pChars, const Size& currentSize, const Size& originalSize) {
                PCharVector pScaledChars = new vector<PCharacterCandidate>(pChars->size());
                CharVector& chars=*pChars;
                CharVector& scaledChars=*pScaledChars;
                for (int i = 0; i < pChars->size(); ++i) {
                    scaledChars[i] = new ScaledCharacter(chars[i], currentSize, originalSize);
                    scaledChars[i]->setPrediction(chars[i]->getPrediction(),chars[i]->getConfidence());
                }
                return pScaledChars;
            }
        };
    }
}
#endif //UIC_SCALEDCHARACTER_H
