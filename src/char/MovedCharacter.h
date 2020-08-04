//
// Created by fab on 06.03.17.
//

#ifndef UIC_SCALEDCHARACTER_H
#define UIC_SCALEDCHARACTER_H

#include "CharacterCandidate.h"

namespace cv {
    namespace text {
        class MovedCharacter : public CharacterCandidate {
            const PCharacterCandidate ch;

        public :
            inline MovedCharacter(PCharacterCandidate ch, Point move)
                    : CharacterCandidate(ch->rect + move, ch->id), ch(ch) {
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
                ch->setPrediction(prediction, confidence);
            }

            inline char getPrediction() const {
                return ch->getPrediction();
            }

            inline void setPrediction(char prediction) {
                ch->setPrediction(prediction);
            }

            static PCharVector from(PCharVector& pChars, Point move);
        };
    }
}
#endif //UIC_SCALEDCHARACTER_H
