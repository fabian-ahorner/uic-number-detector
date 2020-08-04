//
// Created by fab on 06.12.16.
//

#ifndef UIC_ERLINEDETECTOR_H
#define UIC_ERLINEDETECTOR_H

#include "../char/CharacterDetector.h"
#include "LineDetector.h"

namespace cv {
    namespace text {

        /*
         *
         */
        class NMLineDetector : public LineDetector {
            struct line_estimates {
                float top1_a0;
                float top1_a1;
                float top2_a0;
                float top2_a1;
                float bottom1_a0;
                float bottom1_a1;
                float bottom2_a0;
                float bottom2_a1;
                int x_min;
                int x_max;
                int h_max;

                bool operator==(const line_estimates &e) const {
                    return ((top1_a0 == e.top1_a0) && (top1_a1 == e.top1_a1) && (top2_a0 == e.top2_a0) &&
                            (top2_a1 == e.top2_a1) && (bottom1_a0 == e.bottom1_a0) && (bottom1_a1 == e.bottom1_a1) &&
                            (bottom2_a0 == e.bottom2_a0) && (bottom2_a1 == e.bottom2_a1) && (x_min == e.x_min) &&
                            (x_max == e.x_max) && (h_max == e.h_max));
                }
            };

            struct region_pair {
                PCharacterCandidate a;
                PCharacterCandidate b;

                region_pair(PCharacterCandidate &a, PCharacterCandidate &b) : a(a), b(b) {}

                bool operator==(const region_pair &p1) const {
                    return ((p1.a->id == a->id) && (p1.b->id == b->id));
                }
            };

            struct region_triplet {
                PCharacterCandidate a;
                PCharacterCandidate b;
                PCharacterCandidate c;
                line_estimates estimates;

                region_triplet(const PCharacterCandidate &a, const PCharacterCandidate &b, const PCharacterCandidate &c)
                        : a(a), b(b), c(c) {}

                inline region_triplet() {};

                bool operator==(const region_triplet &t1) const {
                    return ((t1.a->id == a->id) && (t1.b->id == b->id) && (t1.c->id == c->id));
                }
            };

            struct region_sequence {
                vector<region_triplet> triplets;

                region_sequence(region_triplet t) {
                    triplets.push_back(t);
                }

                region_sequence() {}
            };

            // Evaluates if a pair of regions is valid or not
            // using thresholds learned on training (defined above)
            bool isValidPair(const PCharacterCandidate &ch1, const PCharacterCandidate &ch2);

            // Evaluates if a set of 3 regions is valid or not
            // using thresholds learned on training (defined above)
            bool isValidTriplet(region_pair pair1, region_pair pair2,
                                region_triplet &triplet);

            // Evaluates if a set of more than 3 regions is valid or not
            // using thresholds learned on training (defined above)
            bool isValidSequence(region_sequence &sequence1, region_sequence &sequence2);

            // Check if two sequences share a region in common
            bool haveCommonRegion(region_sequence &sequence1, region_sequence &sequence2);

            // Check if two triplets share a region in common
            bool haveCommonRegion(region_triplet &t1, region_triplet &t2);

            float distanceLinesEstimates(line_estimates &a, line_estimates &b);

            bool fitLineEstimates(region_triplet &triplet);

            float fitLineLMS(Point p1, Point p2, Point p3, float &a0, float &a1);

            void fitLine(Point p1, Point p2, float &a0, float &a1);


        public:
            NMLineDetector(PCharProcess charDetector) :
                    LineDetector(charDetector) {
            }

            PLineVector process(Mat& original,Mat &img, PCharVector characters);

            inline string getProcessName() {
                return "NMLineDetector";
            }
        };
    }
}

#endif //UIC_ERLINEDETECTOR_H
