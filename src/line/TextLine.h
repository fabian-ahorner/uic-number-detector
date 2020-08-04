//
// Created by fab on 10.11.16.
//
#ifndef UIC_TEXTLINE_H
#define UIC_TEXTLINE_H

#include <string>
#include <vector>
#include "../char/CharacterCandidate.h"

namespace cv {
    namespace text {
        using namespace std;

        class TextLine {
            PCharVector characters = new vector<PCharacterCandidate>();
            string prediction;
            bool _hasPrediction = false;
            float avgWidth = 0, avgHeight = 0;
        public :
            Rect rect;

            inline TextLine() {
            }

            inline TextLine(PCharVector &chars) {
                this->characters = chars;
                for (auto &ch:*chars) {
                    if (rect.width == 0)
                        rect = ch->getRect();
                    else
                        rect |= ch->getRect();
                    avgWidth += ch->width;
                    avgHeight += ch->height;
                }
                avgWidth /= chars->size();
                avgHeight /= chars->size();
            }

            void addChar(Ptr<CharacterCandidate> ch);

            inline void setPrediction(string text) {
                this->prediction = text;
                if (!text.empty())
                    _hasPrediction = true;
            }

            inline string getPrediction() const {
                return prediction;
            }

            Rect getBoundingBox() const;

            int size() const;

            PCharVector &getCharacters();

            const PCharVector &getCharacters() const;

            inline float getAvgWidth() const {
                return avgWidth;
            }

            inline float getAvgHeight() const {
                return avgHeight;
            }

            static inline bool sortByY(const Ptr<TextLine> &l1, const Ptr<TextLine> &l2) {
                return l1->rect.y < l2->rect.y;
            }

            Mat drawLine(const Size &originalSize);

            bool hasPrediction() const;
        };

        typedef Ptr<TextLine> PTextLine;
        typedef vector<PTextLine> LineVector;
        typedef Ptr<LineVector> PLineVector;
//        typedef vector<TextLine> LineVector;
//        typedef Ptr<LineVector> PLineVector;
    }
}
#endif //UIC_TEXTLINE_H
