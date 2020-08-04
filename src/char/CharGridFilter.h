//
// Created by fab on 12.08.17.
//

#ifndef UIC_CHARGRIDFILTER_H
#define UIC_CHARGRIDFILTER_H

#include "CharacterCandidate.h"
#include <math.h>
#include <ImageUtils.h>
#include <queue>
#include <iostream>

namespace cv {
    namespace text {
        struct SortByScore {
            inline bool operator()(const PCharacterCandidate &c1, const PCharacterCandidate &c2) {
                return c1->getIsCharProbability() < c2->getIsCharProbability();
            }
        };

        class CharGrid {
            vector<Ptr<priority_queue<PCharacterCandidate, CharVector, SortByScore>>> grid;
            unsigned int width, height;
            int elementSize;

            int getCellIndex(Point p) {
                return getElementIndex(p.x, p.y);
            }

            int getElementIndex(const int &x, const int &y) {
                return y / elementSize * width + x / elementSize;
            }

        public:
            CharVector collect(int n) {
                CharVector result;
                for (int i = 0; i < width * height; ++i) {
                    Ptr<priority_queue<PCharacterCandidate, CharVector, SortByScore>> cell = grid[i];
                    int take = cell.empty() ? 0 : MIN((int)cell->size(), n);
                    for (int j = 0; j < take; ++j) {
                        result.push_back(cell->top());
                        if(take>1)
                        cell->pop();
                    }
                }
                return result;
            }

            inline CharGrid(Size imgSize, unsigned int scale) {
                elementSize = 2 << scale;
                width = (unsigned int) ceil(imgSize.width / (double) elementSize);
                height = (unsigned int) ceil(imgSize.height / (double) elementSize);
                grid.resize(width * height);
            }

            int add(PCharacterCandidate ch) {
                const Point &c = getCenter(ch->rect);
                int cellIndex = getCellIndex(c);
                Ptr<priority_queue<PCharacterCandidate, CharVector, SortByScore>> cell = grid[cellIndex];
                if (cell.empty()) {
                    cell = new priority_queue<PCharacterCandidate, CharVector, SortByScore>();
                    grid[cellIndex] = cell;
                }
                cell->push(ch);
            }
        };

        class CharGridFilter {
            Size imgSize;
            vector<Ptr<CharGrid>> grids;

            unsigned int getScale(int size);

            inline void add(PCharacterCandidate ch){
                unsigned int scale = getScale(MIN(ch->rect.width,ch->rect.height))-1;
                Ptr <CharGrid> &grid = grids[scale];
                if(grid.empty()){
                    grid = new CharGrid(imgSize,scale);
                    grids[scale]=grid;
                }
                grid->add(ch);
            }

        public:
            inline CharGridFilter(Size imgSize){
                unsigned int size = (unsigned int) ceil(log2(1+MAX(imgSize.width,imgSize.height)));
                grids.resize(size);
                this->imgSize=imgSize;
            }

        public:
            PCharVector filter(PCharVector chars, int n){
                for(auto ch:*chars){
                    add(ch);
                }
                PCharVector result = new CharVector();
                for(auto grid:grids){
                    if(!grid.empty()){
                        CharVector filtered = grid->collect(n);
                        for(auto ch:filtered){
                            result->push_back(ch);
                        }
                    }

                }
                return result;
            }

        };
    }
}

#endif //UIC_CHARGRIDFILTER_H
