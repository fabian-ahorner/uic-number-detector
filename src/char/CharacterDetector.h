//
// Created by fab on 15.11.16.
//

#ifndef UIC_CHARACTEREXTRACTOR_H
#define UIC_CHARACTEREXTRACTOR_H

#include <opencv2/core/core.hpp>
#include "CharacterCandidate.h"
#include "../cv/ImagePipeline.h"
#include "../cv/ImageUtils.h"
#include <vector>
#include <iostream>

using namespace std;
namespace cv {
    namespace text {
        struct CharDetectorOptions {
        public:
            int charsX = 8;
            int charsY = 3;
            virtual inline String toString(){
                std::ostringstream str;
                str << "charsX="<<charsX<<endl;
                str << "charsY="<<charsY<<endl;
                return str.str();
            }
        };
        typedef ImageProcess<PCharVector> CharProcess;
        typedef Ptr<CharProcess> PCharProcess;

        class CharacterDetector : public CharProcess {
        protected:
            const int charsX, charsY;
            int currentId;
        public:
            inline int getNextId() {
                return ++currentId;
            }

            inline int getNextIdAndSkip(int cnt) {
                int nextId = currentId + 1;
                currentId += cnt;
                return nextId;
            }

            inline void skipIds(int cnt) {
                currentId += cnt;
            }

            inline void setStartId(int id) {
                currentId = id;
            }

            inline int getCurrentId() {
                return currentId;
            }

            CharacterDetector(CharDetectorOptions options = CharDetectorOptions()) : charsX(options.charsX),
                                                                                     charsY(options.charsY) {}

            static inline Size getMinSize(int charsX, int charsY) {
                //Char size plus 1px gap in between
                int minW = charsX * CharacterCandidate::MIN_CHAR_SIZE.width + charsX - 1;
                int minH = charsY * CharacterCandidate::MIN_CHAR_SIZE.height + charsY - 1;
                return Size(minW, minH);
            }
//            virtual PCharVector processImg(Mat img, int idFrom)=0;

            virtual PCharVector processImage(Mat original,Mat img)=0;
        };


    }
}
#endif //UIC_CHARACTEREXTRACTOR_H
