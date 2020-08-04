//
// Created by fab on 22.10.16.
//

#include "ImageUtils.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/text.hpp"

namespace cv {

    Mat resizeWithBorder(const Mat &img, int w, int h) {
        float scale = MIN(w / (float) img.rows, h / (float) img.cols);

        Size resizeSize((int) MAX(1,round(img.cols * scale)), (int) MAX(1,round(img.rows * scale)));
        Mat tmp(resizeSize.height,resizeSize.width,CV_8UC1);
        resize(img, tmp, resizeSize);

        Mat out = Mat::zeros(h, w, CV_8UC1);
        Rect to(w / 2 - tmp.cols / 2, h / 2 - tmp.rows / 2, tmp.cols, tmp.rows);
        tmp.copyTo(out(to));
        return out;
    }

    Rect merge(const Rect &r1, const Rect &r2) {
        if (r1.width == 0 || r1.height == 0)
            return r2;
        if (r2.width == 0 || r2.height == 0)
            return r1;
        int x = MIN(r1.x, r2.x);
        int y = MIN(r1.y, r2.y);
        return cv::Rect(x, y,
                        MAX(r1.x + r1.width, r2.x + r2.width) - x,
                        MAX(r1.y + r1.height, r2.y + r2.height) - y
        );
    }

    Point getCenter(const Rect &rect) {
        return Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
    }

    int getRadius(const Rect &rect) {
//        return rect.height / 2;
        return (int) sqrt(pow(rect.height, 2) +
                          pow(rect.width, 2)) / 2;
    }

    void printMat(std::string name, Mat mat) {
        printf("%3s ", name.c_str());
        for (int col = 0; col < mat.cols; col++) {
            printf("%8x ", col);
        }
        printf("\n");
        for (int row = 0; row < mat.rows; row++) {
            printf("%3x ", row);
            for (int col = 0; col < mat.cols; col++) {
                if (mat.type() == CV_8UC1) {
                    uchar val = mat.at<uchar>(row, col);
                    printf("%8x ", val & 0xFFFFFF);
                } else if (mat.type() == CV_32SC1) {
                    int val = mat.at<int>(row, col);
                    printf("%8x ", val & 0xFFFFFF);
                }
            }
            printf("\n");
        }
    }

    using namespace std;

    void ImageUtils::splitInChannels(Mat img, vector<Mat> &channels, int mode) {
//        text::computeNMChannels(img, channels);
        Mat luminance;

        if (mode & (CHANNELS_HSL | CHANNELS_GRADIENT)) {
            Mat hsv;
            cvtColor(img, hsv, COLOR_RGB2HLS);
            vector<Mat> channelsHSV;
            split(hsv, channelsHSV);

            if (mode & (CHANNELS_LUMINANCE | CHANNELS_GRADIENT)) {
                luminance = getChannel(channelsHSV, 1);
                if (mode & CHANNELS_LUMINANCE)
                    channels.push_back(luminance);
            }
            if (mode & CHANNELS_HUE)
                channels.push_back(getChannel(channelsHSV, 0));
            if (mode & CHANNELS_SATURATION)
                channels.push_back(getChannel(channelsHSV, 2));
        }
//        if (mode & CHANNELS_LUMINANCE) {
//            Mat hls;
//            cvtColor(img, hls, COLOR_RGB2HLS);
//            vector<Mat> channelsHLS;
//            split(hls, channelsHLS);
//
//            Mat lightness(img.rows, img.cols, CV_8UC1, 3);
//            channelsHLS.at(1).copyTo(lightness);
//            channels.push_back(lightness);
//        }
        if (mode & CHANNELS_RGB) {
            vector<Mat> channelsRGB;
            split(img, channelsRGB);

            if (mode & CHANNELS_RED)
                channels.push_back(getChannel(channelsRGB, 0));
            if (mode & CHANNELS_GREEN)
                channels.push_back(getChannel(channelsRGB, 1));
            if (mode & CHANNELS_BLUE)
                channels.push_back(getChannel(channelsRGB, 2));
        }

        if (mode & CHANNELS_GRADIENT) {
            Mat gradient_magnitude = Mat_<float>(luminance.size());
            Mat C = Mat_<float>(luminance);

            Mat kernel = (Mat_<float>(1, 3) << -1, 0, 1);
            Mat grad_x;
            filter2D(C, grad_x, -1, kernel, Point(-1, -1), 0, BORDER_DEFAULT);

            Mat kernel2 = (Mat_<float>(3, 1) << -1, 0, 1);
            Mat grad_y;
            filter2D(C, grad_y, -1, kernel2, Point(-1, -1), 0, BORDER_DEFAULT);

            magnitude(grad_x, grad_y, gradient_magnitude);
            gradient_magnitude.convertTo(gradient_magnitude, CV_8UC1);
            channels.push_back(gradient_magnitude);
        }

        if (mode & CHANNELS_Inverted) {
            // Append negative channels to detect ER- (bright regions over dark background)
            int cn = (int) channels.size();
            for (int c = 0; c < cn; c++)
                channels.push_back(255 - channels[c]);
        }
    }

    Rect ImageUtils::scale(Rect rect, double scale) {
        return cv::Rect((int) ceil(rect.x * scale),
                        (int) ceil(rect.y * scale),
                        (int) ceil(rect.width * scale),
                        (int) ceil(rect.height * scale));
    }

    Size ImageUtils::scale(Size rect, double scale) {
        return cv::Size((int) ceil(rect.width * scale), (int) ceil(rect.height * scale));
    }

    Rect ImageUtils::extend(Rect rect, int plus, const Mat &maxBounds) {
        int x = MAX(0, rect.x - plus);
        int y = MAX(0, rect.y - plus);
        int maxW = maxBounds.cols - x;
        int maxH = maxBounds.rows - y;
        return cv::Rect(x, y, MIN(maxW, rect.width + 2 * plus), MIN(maxH, rect.height + 2 * plus));
    }

    Mat ImageUtils::getChannel(vector<Mat> channels, int index) {
        Mat channel(channels[0].rows, channels[0].cols, CV_8UC1);
        channels.at(index).copyTo(channel);
        channels.push_back(channel);
        return channel;
    }
}
