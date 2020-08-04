//
// Created by fab on 22.10.16.
//

#ifndef UIC_IMAGEUTILS_H
#define UIC_IMAGEUTILS_H

#include "opencv2/core/core.hpp"
#include <string>
#include <vector>

namespace cv {

    Mat resizeWithBorder(const Mat &img, int w, int h);

    Rect merge(const Rect &r1, const Rect &r2);

    Point getCenter(const Rect &rect);

    int getRadius(const Rect &rect);

    void printMat(std::string name, Mat mat);

    class ImageUtils {
    public:
        static const int CHANNELS_Inverted = 1<<1;
        static const int CHANNELS_RED = 1<<2;
        static const int CHANNELS_GREEN = 1<<3;
        static const int CHANNELS_BLUE = 1<<4;
        static const int CHANNELS_HUE = 1<<5;
        static const int CHANNELS_SATURATION = 1<<6;
        static const int CHANNELS_LUMINANCE = 1<<7;
        static const int CHANNELS_GRADIENT = 1<<9;
        static const int CHANNELS_RGB = CHANNELS_RED | CHANNELS_GREEN | CHANNELS_BLUE;
        static const int CHANNELS_HSL = CHANNELS_HUE | CHANNELS_SATURATION | CHANNELS_LUMINANCE;
//        static const int CHANNELS_LUMINANCE = 1<<4;
        static const int CHANNELS_ALL =
                ImageUtils::CHANNELS_Inverted | ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_GRADIENT;

        static void splitInChannels(Mat img, std::vector<Mat> &channels, int mode);

        static Rect scale(Rect rect, double scale);

        static Size scale(Size rect, double scale);

        static Rect extend(Rect rect, int plus, const Mat &maxBounds);

        static Mat getChannel(std::vector <Mat> channels, int index);
    };
}


#endif //UIC_IMAGEUTILS_H
