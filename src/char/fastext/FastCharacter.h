//
// Created by fab on 06.12.16.
//

#ifndef UIC_FASTCHARACTER_H
#define UIC_FASTCHARACTER_H

//#include <opencv2/core/core.hpp>
#include "../CharacterCandidate.h"
#include "FastCandidate.h"
//#include <CharacterCandidate.h>

namespace cv {
    namespace text {
        using namespace std;

        template<class VAL>
        class FastCharacter : public CharacterCandidate {
            double getScale() const;

        public:
            const Mat mask, img;
//            const double scale;
            const Ptr<FastCandidate<VAL>> candidate;

            FastCharacter(const Ptr<FastCandidate<VAL>> &candidate, const Rect &rect, const Mat &mask,
                          const Mat &img,
                          int id);

            Mat getImage();

            void draw(Mat img);

            static PCharVector
            from(vector<Ptr<FastCandidate<VAL>>> candidates, const Mat &mask, const Mat &img,
                 int idFrom);

            static Rect computeBox(const Rect &scaledBox, const Size &originalSize, double scaleX, double scaleY);

            double getIsCharProbability();

            double getCharQuality();

            bool isCompatible(const Ptr<CharacterCandidate> &other);
        };

        template<class VAL>
        PCharVector
        FastCharacter<VAL>::from(vector<Ptr<FastCandidate<VAL>>> candidates, const Mat &mask, const Mat &img,
                                 int idFrom) {
            PCharVector
                    characters = new vector<PCharacterCandidate>();
            for (Ptr<FastCandidate<VAL>> &candidate:candidates) {
//                Rect box = computeBox(candidate->getRect(), originalSize, scaleX, scaleY);
                characters->push_back(
                        Ptr<CharacterCandidate>(
                                new FastCharacter<VAL>(candidate, candidate->getRect(), mask, img, idFrom++)));
            }
            return characters;
        }
        template<class VAL>
        FastCharacter<VAL>::FastCharacter(const Ptr<FastCandidate<VAL>> &candidate, const Rect &rect, const Mat &mask,
                                     const Mat &img, int id)
                : CharacterCandidate(rect, id),
                  candidate(candidate),
                  mask(mask),
                  img(img) {
            this->mean = candidate->mean;
            this->standardDeviation = candidate->standardDeviation;
//            assert(this->rect.contains(Point(point.col / scale, point.row / scale)));
        }

        template<class VAL>
        Rect FastCharacter<VAL>::computeBox(const Rect &scaledBox, const Size &originalSize, double scaleX, double scaleY) {
            int boxX = (int) (scaledBox.x / scaleX);
            int boxY = (int) (scaledBox.y / scaleY);
            int boxW = MIN(originalSize.width - boxX, (int) (scaledBox.width / scaleX));
            int boxH = MIN(originalSize.height - boxY, (int) (scaledBox.height / scaleY));
            return cv::Rect(boxX, boxY, boxW, boxH);
        }
        template<class VAL>
        void FastCharacter<VAL>::draw(Mat img) {
            Mat scaled = getImage();
//            namedWindow("debug", CV_WINDOW_NORMAL);
//            imshow("debug", scaled);
//            waitKey();
            Mat originalSize;
            Size s(rect.width, rect.height);
            resize(scaled, originalSize, s);//, 0,0, CV_INTER_LINEAR
            originalSize.copyTo(img(rect));
//            draw(img, Point());
        }
        template<class VAL>
        struct PixelToAdd {
            const VAL *ptrMask;
            uchar *ptrOut;
            Point point;

            PixelToAdd(const VAL *ptrMask, uchar *ptrImg, const Point point) : ptrMask(ptrMask), ptrOut(ptrImg),
                                                                                 point(point) {}
        };

        template<class VAL>
        Mat FastCharacter<VAL>::getImage() {
            Rect originalBounds = candidate->getRect();
            Mat out = Mat::zeros(originalBounds.height, originalBounds.width, CV_8UC1);

            NeighbourVisitor<uchar> outVisitor = createNeighbourVisitor<uchar>(out, NeighbourVisitor_CIRCLE8);
            NeighbourVisitor<VAL> maskVisitor = createNeighbourVisitor<VAL>(this->img, NeighbourVisitor_CIRCLE8);

            const VAL *ptrMask = maskVisitor.ptr(candidate->point->row, candidate->point->col, this->img);
            uchar *ptrOut = outVisitor.ptr(candidate->point->row - originalBounds.y,
                                           candidate->point->col - originalBounds.x, out);
            Point p(candidate->point->getX(), candidate->point->getY());
            ptrOut[0] = 0xFF;

            int area = 0;
//            Rect bounds((int) (rect.x * scale), (int) (rect.y * scale), out.cols, out.rows);
            vector<PixelToAdd<VAL>> toAdd;
            toAdd.push_back(PixelToAdd<VAL>(ptrMask, ptrOut, p));
            do {
                PixelToAdd<VAL> pixel = toAdd.back();
                toAdd.pop_back();
                area++;
                const VAL &val = pixel.ptrMask[0];

                for (int i = 0; i < maskVisitor.size; i++) {
                    p = pixel.point + outVisitor.coordinates[i];
                    ptrOut = pixel.ptrOut + outVisitor.indices[i];
                    if (originalBounds.contains(p)) {
                        if (*ptrOut == 0) {
                            ptrMask = pixel.ptrMask + maskVisitor.indices[i];
                            if (candidate->point->accepts(val, ptrMask[0])) {
                                ptrOut[0] = (uchar) 0xFF;
//                                printMat("Mask", out);
                                toAdd.push_back(PixelToAdd<VAL>(ptrMask, ptrOut, p));
                            }
                        }
                    }
                }
            } while (!toAdd.empty());
//            cout << getPrediction() << ": " << candidate.probability << endl;

//            assert(area == candidate->area);
            return out;
        }
        template<class VAL>
        double FastCharacter<VAL>::getIsCharProbability() {
            return candidate->probability;
        }
        template<class VAL>
        double FastCharacter<VAL>::getCharQuality() {
            return getScale();
        }
        template<class VAL>
        double FastCharacter<VAL>::getScale() const {
            return candidate->getWidth() / (double) rect.width;
        }
        template<class VAL>
        bool FastCharacter<VAL>::isCompatible(const Ptr<CharacterCandidate> &other) {
            double d = norm(getMean() - other->getMean());
            return d < candidate->point->diff;
        }
    }
}

#endif //UIC_FASTCHARACTER_H
