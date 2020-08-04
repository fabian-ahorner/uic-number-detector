//
// Created by fab on 19.02.17.
//

#ifndef UIC_MERGEDCANDIDATE_H
#define UIC_MERGEDCANDIDATE_H
namespace cv {
    namespace text {

        class MergedCharacter : public CharacterCandidate {
            const PCharacterCandidate ch1, ch2;
        public :
            inline MergedCharacter(PCharacterCandidate ch1, PCharacterCandidate ch2)
                    : CharacterCandidate(ch1->rect | ch2->rect, MIN(ch1->id, ch2->id)), ch1(ch1), ch2(ch2) {
            }

            Mat getImage() {
                Mat img = Mat::zeros(height, width, CV_8UC1);
                Point tl = rect.tl();
                Mat ch1Img = ch1->getImage();
                Mat ch2Img = ch2->getImage();
                resize(ch1Img, ch1Img, Size(ch1->width, ch1->height));
                resize(ch2Img, ch2Img, Size(ch2->width, ch2->height));
                img(ch1->rect - tl) += ch1Img;
                img(ch2->rect - tl) += ch2Img;
                return img;
            }

            inline void draw(Mat img) {
                ch1->draw(img);
                ch2->draw(img);
            }

            inline double getIsCharProbability() {
                return MAX(ch1->getIsCharProbability(), ch2->getIsCharProbability());
            }

            inline double getCharQuality() {
                return MAX(ch1->getIsCharProbability(), ch2->getIsCharProbability());
            }
        };
    }
}
#endif //UIC_MERGEDCANDIDATE_H
