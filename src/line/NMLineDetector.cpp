//
// Created by fab on 06.12.16.
//

#include "NMLineDetector.h"
#include "../cv/ImageUtils.h"
#include "LineCandidate.h"
#include <set>
#include <unordered_set>

#define PAIR_MIN_HEIGHT_RATIO     0.4
#define PAIR_MIN_CENTROID_ANGLE - 0.85
#define PAIR_MAX_CENTROID_ANGLE   0.85
#define PAIR_MIN_REGION_DIST    - 0.4
#define PAIR_MAX_REGION_DIST      2.2
//#define PAIR_MAX_INTENSITY_DIST   111
//#define PAIR_MAX_AB_DIST          54
#define PAIR_MAX_LAB_DISTANCE     50

#define TRIPLET_MAX_DIST          0.9
#define TRIPLET_MAX_SLOPE         0.3

#define SEQUENCE_MAX_TRIPLET_DIST 0.45
#define SEQUENCE_MIN_LENGHT       4

namespace cv {
    namespace text {
        /*
         * Find groups of Extremal Regions that are organized as text lines. This function implements
            the grouping algorithm described in:
            Neumann L., Matas J.: Real-Time Scene Text Localization and Recognition, CVPR 2012
            Neumann L., Matas J.: A method for text localization and detection, ACCV 2010
         */
        PLineVector NMLineDetector::process(Mat& original, Mat &img, PCharVector characters) {
            vector<region_pair> valid_pairs;
            //check every possible pair of regions
            for (size_t i = 0; i < characters->size(); i++) {
                vector<int> i_siblings;
                int first_i_sibling_idx = (int) valid_pairs.size();
                for (size_t j = i + 1; j < characters->size(); j++) {
                    // check height ratio, centroid angle and region distance normalized by region width
                    // fall within a given interval
                    if (isValidPair((*characters)[i], (*characters)[j])) {
                        bool isCycle = false;
                        for (size_t k = 0; k < i_siblings.size(); k++) {
                            if (isValidPair((*characters)[j], (*characters)[k])) {
                                // choose as sibling the closer and not the first that was "paired" with i
                                Point i_center = getCenter((*characters)[i]->rect);
                                Point j_center = getCenter((*characters)[j]->rect);
                                Point k_center = getCenter((*characters)[k]->rect);

                                if (norm(i_center - j_center) < norm(i_center - k_center)) {
                                    valid_pairs[first_i_sibling_idx + k] = region_pair((*characters)[i],
                                                                                       (*characters)[j]);
                                    i_siblings[k] = (int) j;
                                }
                                isCycle = true;
                                break;
                            }
                        }
                        if (!isCycle) {
                            valid_pairs.push_back(region_pair((*characters)[i], (*characters)[j]));
                            i_siblings.push_back((int) j);
                            //cout << "Valid pair (" << all_regions[i][0] << ","  << all_regions[i][1] << ") (" << all_regions[j][0] << ","  << all_regions[j][1] << ")" << endl;
                        }
                    }
                }
            }

//            cout << "GroupingNM : detected " << valid_pairs.size() << " valid pairs" << endl;

            vector<region_triplet> valid_triplets;

            //check every possible triplet of regions
            for (size_t i = 0; i < valid_pairs.size(); i++) {
                for (size_t j = i + 1; j < valid_pairs.size(); j++) {
                    // check colinearity rules
                    region_triplet valid_triplet;
                    if (isValidTriplet(valid_pairs[i], valid_pairs[j], valid_triplet)) {
                        valid_triplets.push_back(valid_triplet);
                        //cout << "Valid triplet (" << valid_triplet.a[1] << "," <<  valid_triplet.b[1] << "," <<  valid_triplet.c[1] << ")" << endl;
                    }
                }
            }

//            cout << "GroupingNM : detected " << valid_triplets.size() << " valid triplets" << endl;

            vector<region_sequence> valid_sequences;
            vector<region_sequence> pending_sequences;

            for (size_t i = 0; i < valid_triplets.size(); i++) {
                pending_sequences.push_back(region_sequence(valid_triplets[i]));
            }

            for (size_t i = 0; i < pending_sequences.size(); i++) {
                bool expanded = false;
                for (size_t j = i + 1; j < pending_sequences.size(); j++) {
                    if (isValidSequence(pending_sequences[i], pending_sequences[j])) {
                        expanded = true;
                        pending_sequences[i].triplets.insert(pending_sequences[i].triplets.begin(),
                                                             pending_sequences[j].triplets.begin(),
                                                             pending_sequences[j].triplets.end());
                        pending_sequences.erase(pending_sequences.begin() + j);
                        j--;
                    }
                }
                if (expanded) {
                    valid_sequences.push_back(pending_sequences[i]);
                }
            }

            cout << "GroupingNM : detected " << valid_sequences.size() << " valid valid_sequences" << endl;

            // remove a sequence if one its regions is already grouped within a longer seq
            for (size_t i = 0; i < valid_sequences.size(); i++) {
                for (size_t j = i + 1; j < valid_sequences.size(); j++) {
                    if (haveCommonRegion(valid_sequences[i], valid_sequences[j])) {
                        if (valid_sequences[i].triplets.size() < valid_sequences[j].triplets.size()) {
                            valid_sequences.erase(valid_sequences.begin() + i);
                            i--;
                            break;
                        } else {
                            valid_sequences.erase(valid_sequences.begin() + j);
                            j--;
                        }
                    }
                }
            }


            //cout << "GroupingNM : detected " << valid_sequences.size() << " sequences." << endl;
            PLineVector lines = new vector<PTextLine>();
            // Prepare the sequences for output
            for (size_t i = 0; i < valid_sequences.size(); i++) {
//                set<PCharacterCandidate, SortCharByTransformedX> lineChars;
                unordered_set<int> addedIds;
                PTextLine line= new TextLine();

                for (size_t j = 0; j < valid_sequences[i].triplets.size(); j++) {
                    region_triplet triplet = valid_sequences[i].triplets[j];
                    if (addedIds.find(triplet.a->id) == addedIds.end()) {
                        line->addChar(triplet.a);
                        addedIds.insert(triplet.a->id);
                    }
                    if (addedIds.find(triplet.b->id) == addedIds.end()) {
                        line->addChar(triplet.b);
                        addedIds.insert(triplet.b->id);
                    }
                    if (addedIds.find(triplet.c->id) == addedIds.end()) {
                        line->addChar(triplet.c);
                        addedIds.insert(triplet.c->id);
                    }
                }
                lines->push_back(line);
            }

            unordered_set<int> usedChars;
            PLineVector out = new vector<PTextLine>();
            for(auto& line: *lines){
                CharVector &chars = *line->getCharacters();
                LineCandidate candidate(chars[0]);
                for (int i = 1; i < chars.size(); ++i) {
                    candidate.addLetter(chars[i]);
                }
                candidate.filterCharacters();
                candidate.splitInto(out,usedChars);
            }

            return out;
        }

        bool NMLineDetector::isValidPair(const PCharacterCandidate &ch1, const PCharacterCandidate &ch2) {
            Rect minAreaRect = ch1->rect | ch2->rect;
            // Overlapping regions are not valid pair in any case
            if ((minAreaRect == ch1->rect) ||
                (minAreaRect == ch2->rect))
                return false;

            PCharacterCandidate left, right;
            if (ch1->rect.x < ch2->rect.x) {
                left = ch1;
                right = ch2;
            } else {
                left = ch2;
                right = ch1;
            }

            if (left->rect.x == right->rect.x)
                return false;

            const Rect &rectLeft = left->rect;
            const Rect &rectRight = right->rect;

            float height_ratio = (float) min(rectLeft.height, rectRight.height) /
                                 max(rectLeft.height, rectRight.height);

            Point centerLeft = getCenter(rectLeft);
            Point centerRight = getCenter(rectRight);
            float centroid_angle = atan2((float) (centerRight.y - centerLeft.y),
                                         (float) (centerRight.x - centerLeft.x));

            int avg_width = (rectLeft.width + rectRight.width) / 2;
            float norm_distance = (float) (rectRight.x - (rectLeft.x + rectLeft.width)) / avg_width;

            if ((height_ratio < PAIR_MIN_HEIGHT_RATIO) ||
                (centroid_angle < PAIR_MIN_CENTROID_ANGLE) ||
                (centroid_angle > PAIR_MAX_CENTROID_ANGLE) ||
                (norm_distance < PAIR_MIN_REGION_DIST) ||
                (norm_distance > PAIR_MAX_REGION_DIST))
                return false;

            Scalar mean1 = ch1->getMean();
            Scalar mean2 = ch1->getMean();
            return norm(mean1 - mean2) < PAIR_MAX_LAB_DISTANCE;
        }

// Evaluates if a set of 3 regions is valid or not
// using thresholds learned on training (defined above)
        bool NMLineDetector::isValidTriplet(region_pair pair1, region_pair pair2,
                                            region_triplet &triplet) {

            if (pair1 == pair2)
                return false;

            // At least one region in common is needed
            if ((pair1.a->id == pair2.a->id) || (pair1.a->id == pair2.b->id) || (pair1.b->id == pair2.a->id) ||
                (pair1.b->id == pair2.b->id)) {

                //fill the indexes in the output tripled (sorted)
                if (pair1.a->id == pair2.a->id) {
                    if ((pair1.b->rect.x <= pair1.a->rect.x) &&
                        (pair2.b->rect.x <= pair1.a->rect.x))
                        return false;
                    if ((pair1.b->rect.x >= pair1.a->rect.x) &&
                        (pair2.b->rect.x >= pair1.a->rect.x))
                        return false;

                    triplet.a = (pair1.b->rect.x <
                                 pair2.b->rect.x) ? pair1.b : pair2.b;
                    triplet.b = pair1.a;
                    triplet.c = (pair1.b->rect.x >
                                 pair2.b->rect.x) ? pair1.b : pair2.b;

                } else if (pair1.a->id == pair2.b->id) {
                    if ((pair1.b->rect.x <= pair1.a->rect.x) &&
                        (pair2.a->rect.x <= pair1.a->rect.x))
                        return false;
                    if ((pair1.b->rect.x >= pair1.a->rect.x) &&
                        (pair2.a->rect.x >= pair1.a->rect.x))
                        return false;

                    triplet.a = (pair1.b->rect.x <
                                 pair2.a->rect.x) ? pair1.b : pair2.a;
                    triplet.b = pair1.a;
                    triplet.c = (pair1.b->rect.x >
                                 pair2.a->rect.x) ? pair1.b : pair2.a;

                } else if (pair1.b->id == pair2.a->id) {
                    if ((pair1.a->rect.x <= pair1.b->rect.x) &&
                        (pair2.b->rect.x <= pair1.b->rect.x))
                        return false;
                    if ((pair1.a->rect.x >= pair1.b->rect.x) &&
                        (pair2.b->rect.x >= pair1.b->rect.x))
                        return false;

                    triplet.a = (pair1.a->rect.x <
                                 pair2.b->rect.x) ? pair1.a : pair2.b;
                    triplet.b = pair1.b;
                    triplet.c = (pair1.a->rect.x >
                                 pair2.b->rect.x) ? pair1.a : pair2.b;

                } else if (pair1.b->id == pair2.b->id) {
                    if ((pair1.a->rect.x <= pair1.b->rect.x) &&
                        (pair2.a->rect.x <= pair1.b->rect.x))
                        return false;
                    if ((pair1.a->rect.x >= pair1.b->rect.x) &&
                        (pair2.a->rect.x >= pair1.b->rect.x))
                        return false;

                    triplet.a = (pair1.a->rect.x <
                                 pair2.a->rect.x) ? pair1.a : pair2.a;
                    triplet.b = pair1.b;
                    triplet.c = (pair1.a->rect.x >
                                 pair2.a->rect.x) ? pair1.a : pair2.a;
                }


                if ((triplet.a->rect.x == triplet.b->rect.x) &&
                    (triplet.a->rect.x == triplet.c->rect.x))
                    return false;

                if ((triplet.a->rect.br().x ==
                     triplet.b->rect.br().x) &&
                    (triplet.a->rect.br().x ==
                     triplet.c->rect.br().x))
                    return false;


                if (!fitLineEstimates(triplet))
                    return false;

                if ((triplet.estimates.bottom1_a0 < triplet.estimates.top1_a0) ||
                    (triplet.estimates.bottom1_a0 < triplet.estimates.top2_a0) ||
                    (triplet.estimates.bottom2_a0 < triplet.estimates.top1_a0) ||
                    (triplet.estimates.bottom2_a0 < triplet.estimates.top2_a0))
                    return false;

                int central_height = (int) min(triplet.estimates.bottom1_a0, triplet.estimates.bottom2_a0) -
                                     (int) max(triplet.estimates.top1_a0, triplet.estimates.top2_a0);
                int top_height = (int) abs(triplet.estimates.top1_a0 - triplet.estimates.top2_a0);
                int bottom_height = (int) abs(triplet.estimates.bottom1_a0 - triplet.estimates.bottom2_a0);

                if (central_height == 0)
                    return false;

                float top_height_ratio = (float) top_height / central_height;
                float bottom_height_ratio = (float) bottom_height / central_height;

                if ((top_height_ratio > TRIPLET_MAX_DIST) || (bottom_height_ratio > TRIPLET_MAX_DIST))
                    return false;

                return abs(triplet.estimates.bottom1_a1) <= TRIPLET_MAX_SLOPE;
            }

            return false;
        }

// Evaluates if a set of more than 3 regions is valid or not
// using thresholds learned on training (defined above)
        bool NMLineDetector::isValidSequence(region_sequence &sequence1, region_sequence &sequence2) {
            for (size_t i = 0; i < sequence2.triplets.size(); i++) {
                for (size_t j = 0; j < sequence1.triplets.size(); j++) {
                    if ((distanceLinesEstimates(sequence2.triplets[i].estimates,
                                                sequence1.triplets[j].estimates) < SEQUENCE_MAX_TRIPLET_DIST) &&
                        ((float) max((sequence2.triplets[i].estimates.x_min - sequence1.triplets[j].estimates.x_max),
                                     (sequence1.triplets[j].estimates.x_min - sequence2.triplets[i].estimates.x_max)) /
                         max(sequence2.triplets[i].estimates.h_max, sequence1.triplets[j].estimates.h_max) <
                         3 * PAIR_MAX_REGION_DIST))
                        return true;
                }
            }

            return false;
        }

// Check if two triplets share a region in common
        bool NMLineDetector::haveCommonRegion(region_triplet &t1, region_triplet &t2) {
            return (t1.a->id == t2.a->id) || (t1.a->id == t2.b->id) || (t1.a->id == t2.c->id) ||
                   (t1.b->id == t2.a->id) || (t1.b->id == t2.b->id) || (t1.b->id == t2.c->id) ||
                   (t1.c->id == t2.a->id) || (t1.c->id == t2.b->id) || (t1.c->id == t2.c->id);
        }

// Check if two sequences share a region in common
        bool NMLineDetector::haveCommonRegion(region_sequence &sequence1, region_sequence &sequence2) {
            for (size_t i = 0; i < sequence2.triplets.size(); i++) {
                for (size_t j = 0; j < sequence1.triplets.size(); j++) {
                    if (haveCommonRegion(sequence2.triplets[i], sequence1.triplets[j]))
                        return true;
                }
            }

            return false;
        }

        float
        NMLineDetector::distanceLinesEstimates(NMLineDetector::line_estimates &a, NMLineDetector::line_estimates &b) {
            CV_Assert((a.h_max != 0) && (b.h_max != 0));

            if (a == b)
                return 0.0f;

            int x_min = min(a.x_min, b.x_min);
            int x_max = max(a.x_max, b.x_max);
            int h_max = max(a.h_max, b.h_max);

            float dist_top = FLT_MAX, dist_bottom = FLT_MAX;
            for (int i = 0; i < 2; i++) {
                float top_a0, top_a1, bottom_a0, bottom_a1;
                if (i == 0) {
                    top_a0 = a.top1_a0;
                    top_a1 = a.top1_a1;
                    bottom_a0 = a.bottom1_a0;
                    bottom_a1 = a.bottom1_a1;
                } else {
                    top_a0 = a.top2_a0;
                    top_a1 = a.top2_a1;
                    bottom_a0 = a.bottom2_a0;
                    bottom_a1 = a.bottom2_a1;
                }
                for (int j = 0; j < 2; j++) {
                    float top_b0, top_b1, bottom_b0, bottom_b1;
                    if (j == 0) {
                        top_b0 = b.top1_a0;
                        top_b1 = b.top1_a1;
                        bottom_b0 = b.bottom1_a0;
                        bottom_b1 = b.bottom1_a1;
                    } else {
                        top_b0 = b.top2_a0;
                        top_b1 = b.top2_a1;
                        bottom_b0 = b.bottom2_a0;
                        bottom_b1 = b.bottom2_a1;
                    }

                    float x_min_dist = abs((top_a0 + x_min * top_a1) - (top_b0 + x_min * top_b1));
                    float x_max_dist = abs((top_a0 + x_max * top_a1) - (top_b0 + x_max * top_b1));
                    dist_top = min(dist_top, max(x_min_dist, x_max_dist) / h_max);

                    x_min_dist = abs((bottom_a0 + x_min * bottom_a1) - (bottom_b0 + x_min * bottom_b1));
                    x_max_dist = abs((bottom_a0 + x_max * bottom_a1) - (bottom_b0 + x_max * bottom_b1));
                    dist_bottom = min(dist_bottom, max(x_min_dist, x_max_dist) / h_max);
                }
            }
            return max(dist_top, dist_bottom);
        }

        // Fit a line_estimate to a group of 3 regions
// out triplet.estimates is updated with the new line estimates
        bool NMLineDetector::fitLineEstimates(region_triplet &triplet) {
            vector<Rect> char_boxes;
            char_boxes.push_back(triplet.a->rect);
            char_boxes.push_back(triplet.b->rect);
            char_boxes.push_back(triplet.c->rect);

            triplet.estimates.x_min = min(min(char_boxes[0].tl().x, char_boxes[1].tl().x), char_boxes[2].tl().x);
            triplet.estimates.x_max = max(max(char_boxes[0].br().x, char_boxes[1].br().x), char_boxes[2].br().x);
            triplet.estimates.h_max = max(max(char_boxes[0].height, char_boxes[1].height), char_boxes[2].height);

            // Fit one bottom line
            float err = fitLineLMS(char_boxes[0].br(), char_boxes[1].br(), char_boxes[2].br(),
                                   triplet.estimates.bottom1_a0, triplet.estimates.bottom1_a1);

            if ((triplet.estimates.bottom1_a0 == -1) && (triplet.estimates.bottom1_a1 == 0))
                return false;

            // Slope for all lines must be the same
            triplet.estimates.bottom2_a1 = triplet.estimates.bottom1_a1;
            triplet.estimates.top1_a1 = triplet.estimates.bottom1_a1;
            triplet.estimates.top2_a1 = triplet.estimates.bottom1_a1;

            if (abs(err) > (float) triplet.estimates.h_max / 6) {
                // We need two different bottom lines
                triplet.estimates.bottom2_a0 = triplet.estimates.bottom1_a0 + err;
            } else {
                // Second bottom line is the same
                triplet.estimates.bottom2_a0 = triplet.estimates.bottom1_a0;
            }

            // Fit one top line within the two (Y)-closer coordinates
            int d_12 = abs(char_boxes[0].tl().y - char_boxes[1].tl().y);
            int d_13 = abs(char_boxes[0].tl().y - char_boxes[2].tl().y);
            int d_23 = abs(char_boxes[1].tl().y - char_boxes[2].tl().y);
            if ((d_12 < d_13) && (d_12 < d_23)) {
                Point p = Point((char_boxes[0].tl().x + char_boxes[1].tl().x) / 2,
                                (char_boxes[0].tl().y + char_boxes[1].tl().y) / 2);
                triplet.estimates.top1_a0 = triplet.estimates.bottom1_a0 +
                                            (p.y - (triplet.estimates.bottom1_a0 + p.x * triplet.estimates.bottom1_a1));
                p = char_boxes[2].tl();
                err = (p.y - (triplet.estimates.top1_a0 + p.x * triplet.estimates.top1_a1));
            } else if (d_13 < d_23) {
                Point p = Point((char_boxes[0].tl().x + char_boxes[2].tl().x) / 2,
                                (char_boxes[0].tl().y + char_boxes[2].tl().y) / 2);
                triplet.estimates.top1_a0 = triplet.estimates.bottom1_a0 +
                                            (p.y - (triplet.estimates.bottom1_a0 + p.x * triplet.estimates.bottom1_a1));
                p = char_boxes[1].tl();
                err = (p.y - (triplet.estimates.top1_a0 + p.x * triplet.estimates.top1_a1));
            } else {
                Point p = Point((char_boxes[1].tl().x + char_boxes[2].tl().x) / 2,
                                (char_boxes[1].tl().y + char_boxes[2].tl().y) / 2);
                triplet.estimates.top1_a0 = triplet.estimates.bottom1_a0 +
                                            (p.y - (triplet.estimates.bottom1_a0 + p.x * triplet.estimates.bottom1_a1));
                p = char_boxes[0].tl();
                err = (p.y - (triplet.estimates.top1_a0 + p.x * triplet.estimates.top1_a1));
            }

            if (abs(err) > (float) triplet.estimates.h_max / 6) {
                // We need two different top lines
                triplet.estimates.top2_a0 = triplet.estimates.top1_a0 + err;
            } else {
                // Second top line is the same
                triplet.estimates.top2_a0 = triplet.estimates.top1_a0;
            }

            return true;
        }

        // Fit line from three points using (heutistic) Least-Median of Squares
// out a0 is the intercept
// out a1 is the slope
// returns the error of the single point that doesn't fit the line
        float NMLineDetector::fitLineLMS(Point p1, Point p2, Point p3, float &a0, float &a1) {
            //if this is not changed the line is not valid
            a0 = -1;
            a1 = 0;

            //Least-Median of Squares does not make sense with only three points
            //becuse any line passing by two of them has median_error = 0
            //So we'll take the one with smaller slope
            float l_a0, l_a1, best_slope = FLT_MAX, err = 0;

            if (p1.x != p2.x) {
                fitLine(p1, p2, l_a0, l_a1);;
                if (abs(l_a1) < best_slope) {
                    best_slope = abs(l_a1);
                    a0 = l_a0;
                    a1 = l_a1;
                    err = (p3.y - (a0 + a1 * p3.x));
                }
            }


            if (p1.x != p3.x) {
                fitLine(p1, p3, l_a0, l_a1);
                if (abs(l_a1) < best_slope) {
                    best_slope = abs(l_a1);
                    a0 = l_a0;
                    a1 = l_a1;
                    err = (p2.y - (a0 + a1 * p2.x));
                }
            }


            if (p2.x != p3.x) {
                fitLine(p2, p3, l_a0, l_a1);
                if (abs(l_a1) < best_slope) {
                    best_slope = abs(l_a1);
                    a0 = l_a0;
                    a1 = l_a1;
                    err = (p1.y - (a0 + a1 * p1.x));
                }
            }
            return err;

        }

// Fit line from two points
// out a0 is the intercept
// out a1 is the slope
        void NMLineDetector::fitLine(Point p1, Point p2, float &a0, float &a1) {
            CV_Assert (p1.x != p2.x);

            a1 = (float) (p2.y - p1.y) / (p2.x - p1.x);
            a0 = a1 * -1 * p1.x + p1.y;
        }
    }
}