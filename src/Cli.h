//
// Created by fab on 09.04.17.
//

#ifndef UIC_CLI_H
#define UIC_CLI_H

#include <string>
#include <vector>
#include <UicDetector.h>
#include "opencv2/core.hpp"

using namespace cv::text;
using namespace cv::uic;
using namespace std;

class Cli {
public:
    class Arguments {
    public:
        int colorChannels=0;
        bool printRect=false;
        bool printTimings=false;
        bool gui=false;
        bool debug=false;
        UicEntry::ImageType mode = UicEntry::ImageType::DIGITS;
        vector<string> whitelist;
        string detector;
        string pathImage;
        string pathOut;
        string pathUicEntry;
        string pathDataset;

        static Arguments parseArguments(int argc, char *argv[]);

    private:
        void parseChannels(char *string);

        void parseWhiteList(char *string);

        void parseDetector(char *string);

        static bool endsWith(std::string const &a, std::string const &b);

        void parseMode(char *string);

        void addToWhitelist(string uic);
    };


    static void run(const Arguments &options);

private:

    static void printHelpAndExit(int returnValue = 0);


    static PUicProcess loadDetector(const Arguments &options);

    static void outputResult(Ptr<UicResult> &result, Mat img, const Arguments &arguments);
};


#endif //UIC_CLI_H
