//
// Created by fab on 02.04.17.
//

#ifndef UIC_NMCHARLINEDETECTOR_H
#define UIC_NMCHARLINEDETECTOR_H

#include <opencv/cv.hpp>
#include "../char/erstat/ERStatCandidate.h"
#include "../char/erstat/ERStatDetector.h"
#include "TextLine.h"
#include "LineCandidate.h"

namespace cv {
    namespace text {
        class NMCharLineDetector : public ImageProcess<PLineVector> {
            ErStatOptions options;
        public:
            inline NMCharLineDetector(ErStatOptions erStatOptions) {
                options = erStatOptions;
            }

            string getProcessName() {
                return "NMCharLineDetector";
            };

            PLineVector processImage(Mat original, Mat processed) {
                vector<Mat> channels = {processed};

                Ptr<ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1(options.classifierNM1Path),
                                                             options.minThreshold, 0.0000000001f, 1,
                                                             options.minProbabilityNM1, options.nonMaxSuppression,
                                                             options.minProbabilityThresholdNM1);
                Ptr<ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2(options.classifierNM2Path),
                                                             options.minProbabilityNM2);
                vector<vector<ERStat> > regions(1);
                // Apply the default cascade classifierNM1 to each independent channel (could be done in parallel)
                er_filter1->run(channels[0], regions[0]);
                er_filter2->run(channels[0], regions[0]);

                // Detect character groups
                vector<vector<Vec2i> > region_groups;
                vector<Rect> groups_boxes;
                erGrouping(original, channels, regions, region_groups, groups_boxes, ERGROUPING_ORIENTATION_HORIZ);

                unordered_set<int> set;
                Ptr<Mat> channel = new Mat(processed);
                PLineVector lines = new LineVector();
                for (int l = 0; l < region_groups.size(); ++l) {
                    PTextLine line = new TextLine();
                    LineCandidate lc(new ERCharacter(regions[region_groups[l][0][0]][region_groups[l][0][1]], channel,
                                                     region_groups[l][0][1]));
                    for (int c = 1; c < region_groups[l].size(); ++c) {
                        int index = region_groups[l][c][1];
                        Ptr<ERCharacter> character = new ERCharacter(regions[region_groups[l][c][0]][index], channel,
                                                                     index);
                        lc.addLetter(character);
                        character->computeStats(original);
                        line->addChar(character);
                    }
                    lc.filterCharacters();
                    lc.splitInto(lines,set);
//                    lines->push_back(line);
                }
                return lines;
            }
        };
    }
}

#endif //UIC_NMCHARLINEDETECTOR_H
