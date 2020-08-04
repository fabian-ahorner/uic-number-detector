//
// Created by fab on 19.02.17.
//

#ifndef UIC_ERSTATCANDIDATE_H
#define UIC_ERSTATCANDIDATE_H

namespace cv {
    namespace text {
        using namespace std;

        class ERCharacter : public CharacterCandidate {
            const Ptr<Mat> channel;
        public :
            const ERStat erStat;

            inline void computeStats(const Mat &lab) {
                Mat mask = getImage();
                meanStdDev(lab(rect), mean, standardDeviation, mask);
            }

            inline ERCharacter(const ERStat &erStat, Ptr<Mat> channel, int id)
                    : CharacterCandidate(erStat.rect, id), channel(channel),
                      erStat(erStat) {
            }

            inline Mat getImage() {
                Mat img = Mat::zeros(erStat.rect.height + 2, erStat.rect.width + 2, CV_8UC1);
                int newMaskVal = 255;
                int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
                Mat inputImage = (*channel)(erStat.rect);
                floodFill(inputImage, img,
                          Point(erStat.pixel % channel->cols - erStat.rect.x,
                                erStat.pixel / channel->cols - erStat.rect.y),
                          Scalar(255), 0, Scalar(erStat.level), Scalar(0), flags);
                return img(Rect(1, 1, img.cols - 2, img.rows - 2));
            }

            inline void draw(Mat img) {
                int newMaskVal = 255;
                int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
                Mat inputImage = (*channel)(erStat.rect);

                Rect outRect(erStat.rect.x - 1, erStat.rect.y - 1, erStat.rect.width + 2, erStat.rect.height + 2);
                Point seedPoint = Point(erStat.pixel % channel->cols, erStat.pixel / channel->cols) - erStat.rect.tl();
                if (outRect.x < 0 || outRect.y < 0 || outRect.br().x >= img.cols || outRect.br().y >= img.rows) {
                    Mat outImage = (img)(erStat.rect);
                    copyMakeBorder(outImage, outImage, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));
                    floodFill(inputImage, outImage, seedPoint,
                              Scalar(255), 0, Scalar(erStat.level), Scalar(0), flags);
                    outImage(Rect(1, 1, outImage.cols - 2, outImage.rows - 2)).copyTo(img(erStat.rect));
                } else {
                    Mat outImage = (img)(outRect);
                    floodFill(inputImage, outImage, seedPoint,
                              Scalar(255), 0, Scalar(erStat.level), Scalar(0), flags);
                }
            }

            inline static PCharVector
            from(const Mat &lab, vector<ERStat> erStats, Ptr<Mat> channel, int idFrom) {
                PCharVector characters = new vector<Ptr<CharacterCandidate>>();
                for (auto er:erStats) {
                    ERCharacter *erChar = new ERCharacter(er, channel, idFrom++);
//                    imshow("LAB",lab);
//                    waitKey();
                    erChar->computeStats(lab);
                    characters->push_back(Ptr<CharacterCandidate>(erChar));
                }
                return characters;
            }
            double getIsCharProbability() {
                return erStat.probability;
            }
        };
    }
}

#endif //UIC_ERSTATCANDIDATE_H
