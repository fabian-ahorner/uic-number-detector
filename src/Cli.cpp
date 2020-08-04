//
// Created by fab on 09.04.17.
//

#include <fastext/FasTextDetector.h>
#include <CustomLineDetector.h>
#include <ScaleUicProcess.h>
#include <ChannelUicProcess.h>
#include <UicWindow.h>
#include <UicUtils.h>
#include <NMCharLineDetector.h>
#include "Cli.h"


Cli::Arguments Cli::Arguments::parseArguments(int argc, char *argv[]) {
    argc = 3;
    char *fake[argc];
//    fake[1] = "/home/fab/datasets/uic/full/uic/218024572776_1.jpg";
    fake[1] = "/home/fab/datasets/uic/full/";
    fake[2] = "-g";
//    fake[2] = "-c L";
//    fake[2] = "-c L";
//    fake[3] = "-d fast3";
    argv = fake;
    Arguments options;
    for (int i = 1; i < argc; ++i) {
        string argument = argv[i];
        if (argument[0] == '-') {
            if (argument[1] == '-') {
                if (argument == "--debug") {
                    options.debug = true;
                } else if (argument == "--whitelist") {
                    options.parseWhiteList(argv[++i]);
                } else if (argument == "--print-rect") {
                    options.printRect = true;
                } else if (argument == "--print-timings") {
                    options.printTimings = true;
                } else if (argument == "--detector") {
                    options.parseDetector(argv[++i]);
                } else if (argument == "--channels") {
                    options.parseChannels(argv[++i]);
                } else if (argument == "--region") {
                    options.parseMode(argv[++i]);
                } else if (argument == "--mode") {
                    options.parseMode(argv[++i]);
                } else if (argument == "--gui") {
                    options.gui = true;
                } else if (argument == "--help") {
                    printHelpAndExit();
                } else {
                    cerr << argument << " is not a valid argument" << endl;
                    printHelpAndExit();
                }
            } else {
                switch (argument[1]) {
                    case 'd':
                        options.parseDetector(argv[++i]);
                        break;
                    case 'w':
                        options.parseWhiteList(argv[++i]);
                        break;
                    case 'r':
                        options.printRect = true;
                        break;
                    case 't':
                        options.printTimings = true;
                        break;
                    case 'g':
                        options.gui = true;
                        break;
                    case 'c':
                        options.parseChannels(argv[++i]);
                        break;
                    case 'm':
                        options.parseMode(argv[++i]);
                        break;
                    case 'h':
                        printHelpAndExit();
                        break;
                    default:
                        cerr << argument << " is not a valid argument" << endl;
                        printHelpAndExit(-1);
                }
            }
        } else {
            if (endsWith(argument, ".txt")) {
                options.pathUicEntry = argument;
            } else if (endsWith(argument, ".jpg")) {
                options.pathImage = argument;
            } else if (endsWith(argument, ".png")) {
                options.pathImage = argument;
            } else if (endsWith(argument, ".jpeg")) {
                options.pathImage = argument;
            } else if (endsWith(argument, ".JPEG")) {
                options.pathImage = argument;
            } else {
                options.pathDataset = argument;
            }
        }
    }
    return options;
}

void Cli::run(const Arguments &arguments) {
    if (!arguments.pathImage.empty()) {
        PUicProcess detector = loadDetector(arguments);
        Mat img = imread(arguments.pathImage);
        const PUicResultVector &result = detector->runWithLAB(img);
        if (arguments.printTimings)
            UicDebugHelper::printTimings(detector);
        if (!result->empty()) {
            outputResult((*result)[0], img, arguments);
        }
    } else if (!arguments.pathDataset.empty()) {
        const UicDataset &dataset = UicDatasetHelper::load(arguments.pathDataset);
        if (arguments.gui) {
            UicWindow window(dataset, UicDetectorSelector::createDetectors());
            window.open();
        } else {
            PUicProcess detector = loadDetector(arguments);
            if (arguments.whitelist.empty())
                UicEvaluator::evaluateUicStrings(detector, dataset, arguments.mode);
            else
                UicEvaluator::evaluateUicStrings(detector, dataset, arguments.mode,
                                                 arguments.whitelist);
        }
    } else {
        printHelpAndExit();
    }
}

PUicProcess Cli::loadDetector(const Arguments &options) {
    Ptr<TextDetector> ocrTesseractSingle = new TesseractSingleCharClassifier(true);
    if (options.detector.empty() || options.detector == "fast") {
        FasTextOptions o;
//        return new ScaleUicProcess(new ChannelUicProcess(
//                new SimpleUicDetector(new CustomLineDetector(new FasTextDetector1C(o)), ocrTesseractSingle),
//                o.channels));
        return new ScaleUicProcess(new SimpleUicDetector(new ChannelWrapper<PTextLine>(
                new CustomLineDetector(new FasTextDetector1C(o)),
                options.colorChannels ? options.colorChannels
                                      : o.channels), ocrTesseractSingle));
    } else if (options.detector == "nm") {
        ErStatOptions o;
        return new SimpleUicDetector(new ChannelWrapper<PTextLine>(new NMCharLineDetector(o),
                                                                   options.colorChannels ? options.colorChannels |
                                                                                           ImageUtils::CHANNELS_Inverted
                                                                                         : o.channels),
                                     ocrTesseractSingle);
    } else if (options.detector == "er") {
        ErStatOptions o;
        return new SimpleUicDetector(
                new ChannelWrapper<PTextLine>(new CustomLineDetector(new ErStatDetector(o)),
                                              options.colorChannels ? options.colorChannels |
                                                                      ImageUtils::CHANNELS_Inverted : o.channels),
                ocrTesseractSingle);
    } else if (options.detector == "er") {
        ErStatOptions o;
        return new SimpleUicDetector(
                new ChannelWrapper<PTextLine>(new CustomLineDetector(new ErStatDetector(o)),
                                              options.colorChannels ? options.colorChannels |
                                                                      ImageUtils::CHANNELS_Inverted : o.channels),
                ocrTesseractSingle);
    } else if (options.detector == "fast3") {
        FasTextOptions o;
        return new ScaleUicProcess(
                new SimpleUicDetector(new CustomLineDetector(new FasTextDetector3C(o)), ocrTesseractSingle));
    } else {
        cerr << options.detector << " is not a valid detector" << endl;
        printHelpAndExit(-1);
    }
}

void Cli::printHelpAndExit(int returnValue) {
    cout << "UIC Detector" << endl;
    exit(returnValue);
}

void Cli::outputResult(Ptr<UicResult> &result, Mat img, const Cli::Arguments &arguments) {
    string whitelisted = UicUtils::checkWhitelist(result->uic, arguments.whitelist);
    cout << whitelisted << endl;
    if (arguments.printRect) {
        Rect box = result->getBox();
        printf("%d,%d,%d,%d\r\n", box.x, box.y, box.width, box.height);
    }
    if (!arguments.pathOut.empty()) {
        Mat out = UicDebugHelper::drawUicShape(img, result);
        imwrite(arguments.pathOut, out);
    }

    if (arguments.gui) {
        PLineVector uicLines = new vector<PTextLine>{
                result->lines[0], result->lines[1], result->lines[2]
        };
        UicDebugHelper::show("Shape", UicDebugHelper::drawUicShape(img, result));
        UicDebugHelper::show("Characters", UicDebugHelper::drawSeperateChars(uicLines, Size(img.cols, img.rows)));
        UicDebugHelper::show("UIC", UicDebugHelper::drawUics(img, result));
        waitKey();
    }
}

void Cli::Arguments::parseChannels(char *s) {
    string str(s);
    if (str.find("D") != string::npos)
        colorChannels += ImageUtils::CHANNELS_GRADIENT;
    if (str.find("R") != string::npos)
        colorChannels += ImageUtils::CHANNELS_RED;
    if (str.find("G") != string::npos)
        colorChannels += ImageUtils::CHANNELS_GREEN;
    if (str.find("B") != string::npos)
        colorChannels += ImageUtils::CHANNELS_BLUE;
    if (str.find("H") != string::npos)
        colorChannels += ImageUtils::CHANNELS_HUE;
    if (str.find("S") != string::npos)
        colorChannels += ImageUtils::CHANNELS_SATURATION;
    if (str.find("L") != string::npos)
        colorChannels += ImageUtils::CHANNELS_LUMINANCE;
}

void Cli::Arguments::parseDetector(char *string) {
    this->detector = string;
}

void Cli::Arguments::parseWhiteList(char *s) {
    string str = s;
    if (endsWith(str, ".txt")) {
        ifstream infile(str);
        string line;
        while (std::getline(infile, line)) {
            addToWhitelist(line);
        }
    } else {
        int l = (int) str.length();
        // +1 for delimiter
        if ((l + 1) % 13 == 0) {
            int cnt = (l + 1) / 13;
            for (int i = 0; i < cnt; ++i) {
                if (i > 0 && str[i] != ',') {
                    cerr << str << " is not a valid whitelist" << endl;
                    printHelpAndExit(-1);
                }
                addToWhitelist(string(&str[i * cnt], 12));
            }
        } else {
            cerr << str << " is not a valid whitelist" << endl;
            printHelpAndExit(-1);
        }
//        cout << "Whitelist: " << this->whitelist <<endl ;
    }
}

void Cli::Arguments::parseMode(char *s) {
    string str = s;
    if (str == "digits") {
        this->mode = UicEntry::ImageType::DIGITS;
    } else if (str == "uic") {
        this->mode = UicEntry::ImageType::UIC;
    } else if (str == "train") {
        this->mode = UicEntry::ImageType::TRAIN;
    } else if (str == "original") {
        this->mode = UicEntry::ImageType::TRAIN;
    } else {
        cerr << str << " is not a valid mode" << endl;
        printHelpAndExit(-1);
    }
}

bool Cli::Arguments::endsWith(std::string const &a, std::string const &b) {
    auto len = b.length();
    auto pos = a.length() - len;
    if (pos < 0)
        return false;
    auto pos_a = &a[pos];
    auto pos_b = &b[0];
    while (*pos_a)
        if (*pos_a++ != *pos_b++)
            return false;
    return true;
}

void Cli::Arguments::addToWhitelist(string
uic){
if (!
UicUtils::isValid(uic)
) {
cerr << uic << " is not a valid uic number" << endl <<
flush;
printHelpAndExit(-1);
}
this->whitelist.
push_back(uic);
}
