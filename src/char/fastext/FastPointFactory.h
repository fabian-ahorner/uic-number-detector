//
// Created by fab on 27.01.17.
//

#ifndef UIC_FASTPOINTFACTORY_H
#define UIC_FASTPOINTFACTORY_H

#include "FastPoint.h"
#include "FastCandidate.h"

namespace cv {
    namespace text {
        template<class VAL>
        class FastPointFactory {
        public:
            virtual Ptr<FastPoint<VAL>>
            createFastPoint(int id, VAL foreground, VAL background, int row, int col, float scale, bool isEnd)=0;

            virtual Ptr<FastCandidate<VAL>>
            createFastCandidate(const Ptr<FastPoint<VAL>> &point)=0;

            virtual Scalar subtract(const VAL &val1, const VAL &val2)=0;

            virtual double distance(const VAL &v1, const VAL &v2)=0;

            virtual inline bool hasSameSign(Scalar val1, Scalar val2) {
                return val1.dot(val2) > 0;
            }
        };

        class FastPointFactory1C : public FastPointFactory<uchar> {
        public:
            inline Ptr<FastPoint<uchar>>
            createFastPoint(int id, uchar foreground, uchar background, int row, int col, float scale, bool isEnd) {
                return new FastPoint1C(id, foreground, background, row, col, scale, isEnd);
            }

            inline Ptr<FastCandidate<uchar>>
            createFastCandidate(const Ptr<FastPoint<uchar>> &point) {
                return new FastCandidate<uchar>(point);
            }

            inline Scalar subtract(const uchar &val1, const uchar &val2) {
                return (double) val1 - (double) val2;
            }

            inline double distance(const uchar &val1, const uchar &val2) {
                return (double) val1 - (double) val2;
            }

        };

        class FastPointFactory3C : public FastPointFactory<Vec3b> {
        public:
            inline Ptr<FastPoint<Vec3b>>
            createFastPoint(int id, Vec3b foreground, Vec3b background, int row, int col, float scale, bool isEnd) {
                return new FastPoint3C(id, foreground, background, row, col, scale, isEnd);
            }

            inline Ptr<FastCandidate<Vec3b>>
            createFastCandidate(const Ptr<FastPoint<Vec3b>> &point) {
                return new FastCandidate<Vec3b>(point);
            }

            inline Scalar subtract(const Vec3b &v1, const Vec3b &v2) {
                return Scalar((int) v1[0] - (int) v2[0], (int) v1[1] - (int) v2[1], (int) v1[2] - (int) v2[2]);
            }

            inline double distance(const Vec3b &v1, const Vec3b &v2) {
                return norm(subtract(v1, v2));
            }

            virtual inline bool hasSameSign(Scalar val1, Scalar val2) {
                return true;
            }
        };
    }
}

#endif //UIC_FASTPOINTFACTORY_H
