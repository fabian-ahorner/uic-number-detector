//
// Created by fab on 29.05.17.
//

#ifndef UIC_DISTANCETRANSFORM_H
#define UIC_DISTANCETRANSFORM_H


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <IOUtils.h>

using namespace std;
namespace cv{
    class DistanceTransform {
    public:
        static void doStuff(){
            namedWindow("out", WINDOW_NORMAL);

            const vector<string> &charFiles = cmp::IOUtils::GetFilesInDirectory("/home/fab/datasets/erfilter_train/data/char", "*.tiff", true);

            int cnt=0;
            for(string file: charFiles){
                Mat img = imread(file,0);
                double ratio = getStability(img);
                cout<<"ratio="<<ratio<<endl;
                if(ratio>0.5)
                    cnt++;
            }
            cout<<"CNT="<<cnt<<"/"<<charFiles.size()<<endl;

        }

        static double getStability(Mat img){
            cv::threshold(img, img, 100, 1, cv::THRESH_BINARY);

//            imshow("in",img*100);
            // bw8u : we want to calculate the SWT of this. NOTE: Its background pixels are 0 and forground pixels are 1 (not 255!)
            Mat bw32f, swt32f, kernel;
            double min, max;
            int strokeRadius;

            img.convertTo(bw32f, CV_32F);  // format conversion for multiplication
            distanceTransform(img, swt32f, CV_DIST_L2, 5); // distance transform
            minMaxLoc(swt32f, NULL, &max);  // find max
            strokeRadius = (int)ceil(max);  // half the max stroke width
            kernel = getStructuringElement(MORPH_RECT, Size(3, 3)); // 3x3 kernel used to select 8-connected neighbors

            for (int j = 0; j < strokeRadius; j++)
            {
                dilate(swt32f, swt32f, kernel); // assign the max in 3x3 neighborhood to each center pixel
                swt32f = swt32f.mul(bw32f); // apply mask to restore original shape and to avoid unnecessary max propogation
            }
            // swt32f : resulting SWT image
            imshow("out",swt32f/strokeRadius);

            Scalar mean,stddev;
            meanStdDev ( swt32f, mean, stddev,img );
            cout << "Mean= "<<mean <<endl;
            cout << "stddev= "<<stddev <<endl;
            cout << "ratio= "<<stddev[0]/mean[0] <<endl;
            waitKey(0);
            return stddev[0]/mean[0];
        }
    };
}


#endif //UIC_DISTANCETRANSFORM_H
