//
// Created by fab on 26.11.16.
//

#ifndef UIC_FCHARACTERCANDIDATE_H
#define UIC_FCHARACTERCANDIDATE_H

#include "FastPoint.h"
#include "NeighbourVisitor.h"
#include <opencv2/ml.hpp>


namespace cv {
    namespace text {
        using namespace ml;

        template<class VAL>
        class FastCandidate {
        private:
            inline Scalar vecPow(Scalar vec, int power) {
                double v0 = pow(vec[0], power);
                double v1 = pow(vec[1], power);
                double v2 = pow(vec[2], power);
                return Scalar(v0, v1, v2);
            }

            inline Scalar vecSqrt(Scalar vec) {
                double v0 = sqrt(vec[0]);
                double v1 = sqrt(vec[1]);
                double v2 = sqrt(vec[2]);
                return Scalar(v0, v1, v2);
            }

            struct NeighbourStats {
                short dC[3] = {0, 0, 0};
//                short dC1 = 0, dC2 = 0, dC3 = 0;
                short neighbours = 0, horizontalNeighbours = 0;
            };
            vector<Scalar> values;
            Scalar valSum;
        public :
            static const vector<Point> NEIGHBOURS;
            int minX, maxX, minY, maxY, area = 1;
            double probability;
            const Ptr<FastPoint<VAL>> point;
            double rawMoments[2];     //!< order 1 raw moments to derive the centroid
            double rawCentralMoments[3]; //!< order 2 central moments to construct the covariance matrix
            std::deque<int> crossings;//!< horizontal crossings
            int medianCrossings = 0;
            int perimeter = 0;
            int euler = 0;
            Scalar mean, standardDeviation;
//            ERStat erStat;

            inline FastCandidate(const Ptr<FastPoint<VAL>> &point) : point(point) {
                minX = maxX = point->col;
                minY = maxY = point->row;
                crossings.push_back(2);
                rawMoments[0] = rawMoments[1] = 0;
                rawCentralMoments[0] = rawCentralMoments[1] = rawCentralMoments[2] = 0;
//                erStat = ERStat(0, 0, point.col, point.row);
            }

            void calcNeighbourStats(NeighbourStats &out, Point pos, char *mask, NeighbourVisitor<char> visitor);

            void addPixel(const Vec3b &val, const Point &pos, char *maskPtr, const NeighbourVisitor<char> &visitor);

            void addNeighbour(const Vec3b &val, const Point &pos) {
//                ERFilterNM::er_add_pixle
            }

            inline Rect getRect() const {
                return Rect(minX, minY, maxX - minX + 1, maxY - minY + 1);
            }

            inline int getWidth() const {
                return maxX - minX;
            }

            inline int getHeight() const {
                return maxY - minY;
            }

            void computeNM1Features();
            void computeNM2Features(const Mat& mask);

            Mat getSampleNM1();
            Mat getSampleNM2();

            void updateProbabilityNM1(Ptr<StatModel> classifier);
            void updateProbabilityNM2(Ptr<StatModel> classifier);

            void setProbability(double probability);

            float hole_area_ratio;
            float convex_hull_ratio;
            float num_inflexion_points;
        };
        template<class VAL>
        const vector<Point> FastCandidate<VAL>::NEIGHBOURS = {
                Point(1, 0),
                Point(1, 1),
                Point(0, 1),
                Point(-1, 1),
                Point(-1, 0),
                Point(-1, -1),
                Point(0, -1),
                Point(1, -1)
        };

        template<class VAL>
        void FastCandidate<VAL>::calcNeighbourStats(FastCandidate::NeighbourStats &out, Point pos, char *mask,
                                                    NeighbourVisitor<char> visitor) {
            unsigned char quads[3][4];
            quads[0][0] = 1 << 3;
            quads[0][1] = 1 << 2;
            quads[0][2] = 1 << 1;
            quads[0][3] = 1;
            quads[1][0] = (1 << 2) | (1 << 1) | (1);
            quads[1][1] = (1 << 3) | (1 << 1) | (1);
            quads[1][2] = (1 << 3) | (1 << 2) | (1);
            quads[1][3] = (1 << 3) | (1 << 2) | (1 << 1);
            quads[2][0] = (1 << 2) | (1 << 1);
            quads[2][1] = (1 << 3) | (1);

            unsigned char quad_before[4] = {0, 0, 0, 0};
            unsigned char quad_after[4] = {0, 0, 0, 0};
            quad_after[0] = 1 << 1;
            quad_after[1] = 1 << 3;
            quad_after[2] = 1 << 2;
            quad_after[3] = 1;

            for (int edge = 0; edge < 8; edge++) {
                char *neighbour = visitor.neighbour(mask, edge);
                bool isCharPixel = neighbour != nullptr && neighbour[0] != 0;

                if (isCharPixel) {
                    switch (edge) {
                        case 0:
                            quad_before[3] = quad_before[3] | (1 << 3);
                            quad_after[3] = quad_after[3] | (1 << 3);
                            break;
                        case 6:
                            quad_before[3] = quad_before[3] | (1 << 2);
                            quad_after[3] = quad_after[3] | (1 << 2);
                            quad_before[0] = quad_before[0] | (1 << 3);
                            quad_after[0] = quad_after[0] | (1 << 3);
                            break;
                        case 7:
                            quad_before[0] = quad_before[0] | (1 << 2);
                            quad_after[0] = quad_after[0] | (1 << 2);
                            break;
                        case 4:
                            quad_before[3] = quad_before[3] | (1 << 1);
                            quad_after[3] = quad_after[3] | (1 << 1);
                            quad_before[2] = quad_before[2] | (1 << 3);
                            quad_after[2] = quad_after[2] | (1 << 3);
                            break;
                        case 5:
                            quad_before[0] = quad_before[0] | (1);
                            quad_after[0] = quad_after[0] | (1);
                            quad_before[1] = quad_before[1] | (1 << 2);
                            quad_after[1] = quad_after[1] | (1 << 2);
                            break;
                        case 3:
                            quad_before[2] = quad_before[2] | (1 << 1);
                            quad_after[2] = quad_after[2] | (1 << 1);
                            break;
                        case 2:
                            quad_before[2] = quad_before[2] | (1);
                            quad_after[2] = quad_after[2] | (1);
                            quad_before[1] = quad_before[1] | (1 << 1);
                            quad_after[1] = quad_after[1] | (1 << 1);
                            break;
                        case 1:
                        default:
                            quad_before[1] = quad_before[1] | (1);
                            quad_after[1] = quad_after[1] | (1);
                            break;
                    }
                    if (edge % 2 == 0) {
                        // Is direct neighbour;
                        out.neighbours++;
                        // Is horizontal neighbour
                        if (edge == 0 || edge == 4)
                            out.horizontalNeighbours++;
                    }
                }
            }
            out.dC[0] = out.dC[1] = out.dC[2] = 0;
            for (int p = 0; p < 3; p++) {
                for (int q = 0; q < 4; q++) {
                    if ((p < 2) || (q < 2)) {
                        if (quad_before[0] == quads[p][q])
                            out.dC[p]--;
                        if (quad_before[1] == quads[p][q])
                            out.dC[p]--;
                        if (quad_before[2] == quads[p][q])
                            out.dC[p]--;
                        if (quad_before[3] == quads[p][q])
                            out.dC[p]--;

                        if (quad_after[0] == quads[p][q])
                            out.dC[p]++;
                        if (quad_after[1] == quads[p][q])
                            out.dC[p]++;
                        if (quad_after[2] == quads[p][q])
                            out.dC[p]++;
                        if (quad_after[3] == quads[p][q])
                            out.dC[p]++;
                    }
                }
            }
        }

        template<class VAL>
        void FastCandidate<VAL>::addPixel(const Vec3b &val, const Point &pos, char *maskPtr,
                                          const NeighbourVisitor<char> &visitor) {
//            values.push_back(val);
            valSum[0] += val[0];
            valSum[1] += val[1];
            valSum[2] += val[2];

            int x = pos.x;
            int y = pos.y;
            NeighbourStats neighbourStats;
            calcNeighbourStats(neighbourStats, pos, maskPtr, visitor);

            perimeter += 4 - 2 * neighbourStats.neighbours;

            if (crossings.size() > 0) {
                if (y < minY)
                    crossings.push_front(2);
                else if (y > maxY)
                    crossings.push_back(2);
                else {
                    crossings.at(y - minY) += 2 - 2 * neighbourStats.horizontalNeighbours;
                }
            } else {
                crossings.push_back(2);
            }

            euler += (neighbourStats.dC[0] - neighbourStats.dC[1] + 2 * neighbourStats.dC[2]) / 4;

            rawMoments[0] += x;
            rawMoments[1] += y;
            rawCentralMoments[0] += x * x;
            rawCentralMoments[1] += x * y;
            rawCentralMoments[2] += y * y;

            minX = MIN(minX, pos.x);
            minY = MIN(minY, pos.y);
            maxX = MAX(maxX, pos.x);
            maxY = MAX(maxY, pos.y);
            area++;
        }

        template<class VAL>
        void FastCandidate<VAL>::computeNM1Features() {
            int height = maxY - minY + 1;
            vector<int> m_crossings;
            m_crossings.push_back(crossings.at((unsigned long) (height / 6)));
            m_crossings.push_back(crossings.at((unsigned long) (3 * height / 6)));
            m_crossings.push_back(crossings.at((unsigned long) (5 * height / 6)));
            sort(m_crossings.begin(), m_crossings.end());
            medianCrossings = m_crossings.at(1);

            this->mean = valSum / area;
//            valSum = Scalar();
//            for (auto &val:values) {
//                valSum += vecPow(mean - val, 2);
//            }
//            this->standardDeviation = vecSqrt(valSum);
        }

        template<class VAL>
        Mat FastCandidate<VAL>::getSampleNM1() {
            int height = maxY - minY + 1;
            int width = maxX - minX + 1;
            Mat sample = Mat::zeros(1, 4, CV_32F);
            sample.at<float>(0, 0) = (float) (width) / (height);// aspect ratio
            sample.at<float>(0, 1) = sqrt((float) (area)) / perimeter;// compactness
            sample.at<float>(0, 2) = (float) (1 - euler);//number of holes
            sample.at<float>(0, 3) = (float) medianCrossings;
            return sample;
        }
        template<class VAL>
        Mat FastCandidate<VAL>::getSampleNM2() {
            int height = maxY - minY + 1;
            int width = maxX - minX + 1;
            Mat sample = Mat::zeros(1, 7, CV_32F);
            sample.at<float>(0, 0) = (float) (width) / (height);// aspect ratio
            sample.at<float>(0, 1) = sqrt((float) (area)) / perimeter;// compactness
            sample.at<float>(0, 2) = (float) (1 - euler);//number of holes
            sample.at<float>(0, 3) = (float) medianCrossings;
            sample.at<float>(0, 4) = hole_area_ratio;
            sample.at<float>(0, 5) = convex_hull_ratio;
            sample.at<float>(0, 6) = num_inflexion_points;
            return sample;
        }

        template<class VAL>
        void FastCandidate<VAL>::computeNM2Features(const Mat& mask){
            const Rect &rect = ImageUtils::extend(getRect(),1,mask);
            const Mat& region = mask(rect);

            vector<vector<Point> > contours;
            vector<Point> contour_poly;
            vector<Vec4i> hierarchy;
            findContours( region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0) );
//            //TODO check epsilon parameter of approxPolyDP (set empirically) : we want more precission
//            //     if the region is very small because otherwise we'll loose all the convexities
            approxPolyDP( Mat(contours[0]), contour_poly, (float)min(rect.width,rect.height)/17, true );

            bool was_convex = false;
            int  num_inflexion_points = 0;

            for (int p = 0 ; p<(int)contour_poly.size(); p++)
            {
                int p_prev = p-1;
                int p_next = p+1;
                if (p_prev == -1)
                    p_prev = (int)contour_poly.size()-1;
                if (p_next == (int)contour_poly.size())
                    p_next = 0;

                double angle_next = atan2((double)(contour_poly[p_next].y-contour_poly[p].y),
                                          (double)(contour_poly[p_next].x-contour_poly[p].x));
                double angle_prev = atan2((double)(contour_poly[p_prev].y-contour_poly[p].y),
                                          (double)(contour_poly[p_prev].x-contour_poly[p].x));
                if ( angle_next < 0 )
                    angle_next = 2.*CV_PI + angle_next;

                double angle = (angle_next - angle_prev);
                if (angle > 2.*CV_PI)
                    angle = angle - 2.*CV_PI;
                else if (angle < 0)
                    angle = 2.*CV_PI + abs(angle);

                if (p>0)
                {
                    if ( ((angle > CV_PI)&&(!was_convex)) || ((angle < CV_PI)&&(was_convex)) )
                        num_inflexion_points++;
                }
                was_convex = (angle > CV_PI);

            }

            Mat tmp;
            region.copyTo(tmp);
            floodFill(tmp, Point(0,0), Scalar(255), 0);
            int holes_area = tmp.cols*tmp.rows-countNonZero(tmp);

            int hull_area = 0;

            {

                vector<Point> hull;
                convexHull(contours[0], hull, false);
                hull_area = (int)contourArea(hull);
            }



            this->hole_area_ratio = (float)holes_area / area;
            this->convex_hull_ratio = (float)hull_area / (float)contourArea(contours[0]);
            this->num_inflexion_points = (float)num_inflexion_points;
        }

        template<class VAL>
        void FastCandidate<VAL>::setProbability(double probability) {
            this->probability = probability;
        }

        template<class VAL>
        void FastCandidate<VAL>::updateProbabilityNM1(Ptr<StatModel> classifier) {
            float votes = classifier->predict(getSampleNM1(), noArray(), DTrees::PREDICT_SUM | StatModel::RAW_OUTPUT);
            // Logistic Correction returns a probability value (in the range(0,1))
            setProbability((double) 1 - (double) 1 / (1 + exp(-2 * votes)));
        }

        template<class VAL>
        void FastCandidate<VAL>::updateProbabilityNM2(Ptr<StatModel> classifier) {
            float votes = classifier->predict(getSampleNM2(), noArray(), DTrees::PREDICT_SUM | StatModel::RAW_OUTPUT);
            // Logistic Correction returns a probability value (in the range(0,1))
            setProbability((double) 1 - (double) 1 / (1 + exp(-2 * votes)));
        }
    }
}


#endif //UIC_FCHARACTERCANDIDATE_H
