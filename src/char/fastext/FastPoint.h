//
// Created by fab on 26.11.16.
//

#ifndef UIC_FASTPOINT_H
#define UIC_FASTPOINT_H

#include <opencv2/opencv.hpp>

namespace cv {
    namespace text {
        template<class VAL>
        class FastPoint {
        public:

            float scale;
            bool suppressed = false;
            VAL background;
            VAL foreground;
            double diff;
            double threshold;
            int id;
            int col, row;
            bool isEnd;


            FastPoint(int id, VAL foreground, VAL background, int row, int col, float scale, bool isEnd, double diff) :
                    id(id), foreground(foreground), background(background), row(row), col(col), scale(scale),
                    isEnd(isEnd), diff(diff), threshold(diff - 1) {}


            virtual bool accepts(const VAL &neighbourVal, const VAL val) const =0;

            inline int getX() const {
                return col;
            }

            inline int getY() const {
                return row;
            }
        };

        class FastPoint3C : public FastPoint<Vec3b> {
        private:
            Scalar diff3;

            static inline Scalar subtract(const Vec3b &v1, const Vec3b &v2) {
                return Scalar((int) v1[0] - (int) v2[0], (int) v1[1] - (int) v2[1], (int) v1[2] - (int) v2[2]);
            }

            static inline double distance(const Vec3b &v1, const Vec3b &v2) {
                return norm(subtract(v1, v2));
            }

            static inline bool hasSameSign(Scalar val1, Scalar val2) {
                return val1.dot(val2) > 0;
            }

        public:

            FastPoint3C(int id, Vec3b foreground, Vec3b background, int row, int col, float scale, bool isEnd) :
                    FastPoint<Vec3b>(id, foreground, background, row, col, scale, isEnd,
                                     distance(foreground, background)), diff3(subtract(foreground, background)) {
            }

            inline bool operator<(const FastPoint &other) const {
                return diff < other.diff;
            }


            inline bool accepts(const Vec3b &neighbourVal, const Vec3b val) const {
                if (hasSameSign(diff3, subtract(foreground, val))) {
                    double diff = distance(val, foreground);
                    return diff < threshold;
                } else {
                    return true;
                }
            }
        };

        class FastPoint1C : public FastPoint<uchar> {
        public:

            FastPoint1C(int id, uchar foreground, uchar background, int row, int col, float scale, bool isEnd) :
                    FastPoint<uchar>(id, foreground, background, row, col, scale, isEnd,
                                     distance(foreground, background)) {
            }

            inline bool operator<(const FastPoint &other) const {
                return diff < other.diff;
            }

            static inline double distance(const uchar &v1, const uchar &v2) {
                return abs(v1 - v2);
            }

            inline bool accepts(const uchar &neighbourVal, const uchar val) const {
                if (this->foreground > this->background)
                    return val > this->background;
                else
                    return val < this->background;
//                double diff = distance(val, foreground);
//                return diff < threshold;
            }
        };
    }
}


#endif //UIC_FASTPOINT_H
