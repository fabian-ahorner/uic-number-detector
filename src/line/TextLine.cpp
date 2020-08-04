//
// Created by fab on 10.11.16.
//

#include <iostream>
#include "TextLine.h"
#include "../char/CharMerger.h"
#include "../char/MergedCandidate.h"

namespace cv {
    namespace text {
        void TextLine::addChar(Ptr<CharacterCandidate> ch) {
            if (characters->empty()) {
                rect = ch->getRect();
            } else {
                rect |= ch->getRect();
                PCharacterCandidate last = characters->back();
                float aspectRatio = ch->height / (float) ch->width;
                float lastAspectRatio = last->height / (float) last->width;
                if (aspectRatio > 3 && lastAspectRatio > 3) {
                    Rect mergedRect = ch->rect | last->rect;
                    float aspectRatioMerged = mergedRect.height / (float) mergedRect.width;
                    if (aspectRatioMerged > 1.5 && CharMerger::areLinked(ch, last)) {
                        ch = new MergedCharacter(last, ch);
                        if (characters->size() == 1) {
                            avgWidth = 0;
                            avgHeight = 0;
                        } else {
                            avgWidth = (avgWidth * characters->size() - last->width) / (characters->size() - 1);
                            avgHeight = (avgHeight * characters->size() - last->height) / (characters->size() - 1);
                        }
                        characters->pop_back();
                    }
                }
            }
            characters->push_back(ch);
            avgWidth = (avgWidth * (characters->size() - 1) + ch->width) / characters->size();
            avgHeight = (avgHeight * (characters->size() - 1) + ch->height) / characters->size();
        }

        Rect TextLine::getBoundingBox() const {
            return rect;
        }

        PCharVector &TextLine::getCharacters() {
            return characters;
        }

        const PCharVector &TextLine::getCharacters() const {
            return characters;
        }

        int TextLine::size() const {
            return (int)characters->size();
        }

        Mat TextLine::drawLine(const Size &originalSize) {
            Mat lineImg = Mat::zeros(originalSize.height + 2, originalSize.width + 2, CV_8UC1);
            for (auto &ch:*characters) {
                ch->draw(lineImg);
            }
            Rect roi = rect + Point(1, 1);
            return lineImg(roi);
        }

        bool TextLine::hasPrediction() const {
            return _hasPrediction;
        }
    }
}