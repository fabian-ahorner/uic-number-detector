//
// Created by fab on 30.01.17.
//

#include "CharMerger.h"
#include "../utils/RingBuffer.h"
#include "MergedCandidate.h"

namespace cv {
    namespace text {
        void CharMerger::mergeChars(deque<PCharacterCandidate> &original) {
            vector<PCharacterCandidate> characters;
            for (auto &ch:original) {
                characters.push_back(ch);
            }
            mergeChars(characters);
            original.clear();
            for (auto &ch:characters) {
                original.push_back(ch);
            }
        }

        void CharMerger::mergeChars(vector<PCharacterCandidate> &characters) {
            const double SINGLE_AR = 3;
            const double MERGED_AR = 1.5;

            RingBuffer<PCharacterCandidate> buffer(2);
            for (auto it = characters.begin(); it != characters.end(); it++) {
                buffer.push(*it);
                if (buffer.has(1)) {
                    if (aspectRatio(buffer[0]) > SINGLE_AR && aspectRatio(buffer[1]) > SINGLE_AR) {
                        if (aspectRatio(buffer[0]->rect | buffer[1]->rect) > MERGED_AR) {
//                            if (areLinked(buffer[1], buffer[2])) {
                            PCharacterCandidate merged = new MergedCharacter(buffer[0], buffer[1]);
                            buffer.clear();
                            buffer.push(merged);
                            characters.erase(it);
                            characters.erase(--it);
                            characters.insert(it, merged);
//                            it++;
//                            }
                        }
                    }
                }
            }
//            if (buffer.has(1) && aspectRatio(buffer[1]) > SINGLE_AR && aspectRatio(buffer[0]) > SINGLE_AR
//                && aspectRatio(buffer[0]->rect | buffer[1]->rect) > MERGED_AR) {
//                characters.pop_back();
//                characters.pop_back();
//                PCharacterCandidate merged = new MergedCharacter(buffer[0], buffer[1]);
//                characters.push_back(merged);
//            }
        }

        bool CharMerger::areLinked(const PCharacterCandidate &ch1, const PCharacterCandidate ch2) {
            Mat img1 = ch1->getImage();
            Mat img2 = ch2->getImage();

            int topDir1 = getDirection(img1.ptr<uchar>(0), img1.cols);
            int topDir2 = getDirection(img2.ptr<uchar>(0), img2.cols);
            if (topDir1 != topDir2 && (topDir1 == -1 || topDir2 == +1))
                return true;

            int bottomDir1 = getDirection(img1.ptr<uchar>(img1.rows - 1), img1.cols);
            int bottomDir2 = getDirection(img2.ptr<uchar>(img2.rows - 1), img2.cols);
            if (bottomDir1 != bottomDir2 && (bottomDir1 == -1 || bottomDir2 == +1))
                return true;

            return false;
        }

        int CharMerger::getDirection(uchar *scanLine, int cols) {
            int left = 0;
            int right = 0;
            double half = (cols - 1) / 2.;
            for (int i = 0; i < cols; ++i) {
                if (i < half)
                    left += scanLine[i];
                if (i > half)
                    right += scanLine[i];
            }
            if (left > right)
                return -1;
            if (left < right)
                return +1;
            return 0;
        }

        double CharMerger::aspectRatio(const PCharacterCandidate &ch) {
            return ch->height / (double) ch->width;
        }

        double CharMerger::aspectRatio(const Rect &r) {
            return r.height / (double) r.width;
        }
    }
}