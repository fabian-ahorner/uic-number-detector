//
// Created by fab on 10.10.16.
//

#ifndef UIC_UICDATASET_H
#define UIC_UICDATASET_H

#include <string>
#include <vector>
#include "opencv2/core/core.hpp"

namespace cv {
    namespace uic {
        using namespace std;
        using namespace cv;

        class UicEntry {

        public:
            enum ImageType {
                ORIGINAL, TRAIN, UIC, DIGITS
            };
            string name;
            string uic;
            int quality;
            Size_<double> size;
            Rect_<double> rectTrain;
            Rect_<double> rectUic;
            Rect_<double> rectDigits;
            vector<Rect_<double>> digitBoxes;
            vector<Rect_<double>> digitRects;
            string timeRecorded;
            string cameraType;
            double scale;

            const string getOriginalImage() const;

            const string getTrainImage() const;

            const string getUicImage() const;

            string getImagePath(ImageType type) const;

            Mat getImage(ImageType type) const;

            UicEntry(const string &, const string &);

            static inline bool sortByName(const UicEntry &lhs, const UicEntry &rhs) {
                return lhs.name < rhs.name;
            }

            static inline bool sortByDate(const UicEntry &lhs, const UicEntry &rhs) {
                return lhs.timeRecorded < rhs.timeRecorded;
            }

            Rect getUicBox(ImageType type) const;
            Rect getDigitBox(ImageType type) const;

            Rect_<double> translate(Rect_<double> rect, ImageType mode) const;

            int getMinDigitHeight() const;

            Scalar getDigitRMS() const;

            Rect getRect(ImageType type) const;

        private:
            string path;

            void loadDataFile();

            void readRectangle(ifstream &, Rect_<double> &);

            void computeDigitRects();

            void addDigitsFor(Rect_<double> &digitBox, int digits);

            Mat preProcess(const Mat &mat) const;

            void scaleAll();
        };

        typedef vector<UicEntry> UicDataset;

        static inline vector<string> getWhitelist(const UicDataset &dataset) {
            vector <string> whitelist;
            for (const UicEntry &uic:dataset) {
                whitelist.push_back(uic.uic);
            }
            return whitelist;
        }

        struct UicDatasetHelper {
        public:
            const UicDataset full;
            const UicDataset train;
            const UicDataset test;

            static UicDataset load(string path);

            static UicDataset sortByName(const UicDataset &dataset);

            static UicDataset loadAndShuffle(string path);

            static UicDataset splitDataset(const UicDataset &dataset, double from, double to);

            UicDatasetHelper(string path, double from, double to) : full(loadAndShuffle(path)),
                                                          train(splitDataset(full, from, to)),
                                                          test(splitDataset(full, to, 1)) {
            }
        };

//        class UicDataset {
//            string path;
//            vector<UicEntry> entries;
//        public:
//
//            UicDataset(string path);
//
//            inline const UicEntry &operator[](int x) const {
//                return entries[x];
//            }
//
//            inline unsigned long size() const {
//                return entries.size();
//            }
//        };

//        inline std::ostream &operator<<(std::ostream &strm, const UicEntry &a) {
//            return strm << a.name;
//        }
    }
}
#endif //UIC_UICDATASET_H
