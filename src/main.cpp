#include <opencv2/opencv.hpp>
#include <ScaleUicProcess.h>
#include <ChannelWrapper.h>
#include <UicCharDetector.h>
#include <DistanceTransform.h>
#include <test/CharDetectorEvaluator.h>
#include <NMCharLineDetector.h>
#include <CharClassifier.h>
#include "char/fastext/FasTextDetector.h"
#include "char/ChannelCharacterDetector.h"
#include "char/GridFilterProcess.h"
#include "char/CharScalePiepline.h"
#include "uic/UicEvaluator.h"
#include "line/CustomLineDetector.h"
#include "line/LineCharacterDetector.h"
#include "line/ChannelLineDetector.h"
#include "Cli.h"
#include "results.h"

using namespace cv::text;
using namespace cv::uic;
using namespace cv;

double STATIC_VALUE = 0.8;

int main(int argc, char *argv[]) {
    UicDebugHelper::imagePath = "/home/fab/dev/TEX/img/results/uics/";

//    CharClassifier::createClassifier("/home/fab/datasets/char/font","/home/fab/dev/cpp/uic/models/character_classifier_test.xml");
//    return 0;
//    DistanceTransform::doStuff();W
//    return 0;

    computeResults();
    return 0;

    Cli::run(Cli::Arguments::parseArguments(argc,argv));
    return 0;

    UicDatasetHelper datasets("/data", 0, 0.5);
//    evaluator.log(false);
    cout << datasets.train.size() << endl;

    FasTextOptions fastTextOptions;

    CharDetectorReport report;

    vector<double> values = {
//    0.5,0.6,0.7,0.8,0.9,1
//            1,1.5,2,3,4,5,6,8,12
//0.1,0.5,1
//    0.5,1,1.5,2,2.5,3
//    10,20,40,60,80,100,120,140
            1
//            64, 32, 16, 14, 12, 10, 8, 4, 2, 1
//            1.2, 1.4, 1.6, 1.8, 2
//            0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1
//            33,3,5,37,7,17,49,19,15,57,27,45
//            1,2,4,8,12,16,20,32,64
//    0
//    1
//    ImageUtils::CHANNELS_RED,
//    ImageUtils::CHANNELS_BLUE,
//    ImageUtils::CHANNELS_GREEN,
//    ImageUtils::CHANNELS_HUE,
//    ImageUtils::CHANNELS_SATURATION,
//            ImageUtils::CHANNELS_LUMINANCE,
//    ImageUtils::CHANNELS_GRADIENT,
//    ImageUtils::CHANNELS_RGB,
//    ImageUtils::CHANNELS_HSL,
//    ImageUtils::CHANNELS_GRADIENT | ImageUtils::CHANNELS_RGB,
//    ImageUtils::CHANNELS_GRADIENT | ImageUtils::CHANNELS_HSL,
//    ImageUtils::CHANNELS_GRADIENT | ImageUtils::CHANNELS_HSL | ImageUtils::CHANNELS_RGB,
//    ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_LUMINANCE,
    };

    UicDebugHelper::imageIndex = 0;
    CharDetectorEvaluator evaluator(datasets.train, Ptr<TesseractSingleCharClassifier>(new TesseractSingleCharClassifier(true)));//
    for (int i = 0; i < values.size(); i++) {
        auto v = values[i];
//        LineCandidate::HEIGHT_SIMILARITY = 0.8;
//        LineCandidate::COLOR_SIMILARITY = v;
//        LineCandidate::CENTER_SIMILARITY = 2;
//        LineCandidate::OVERLAP_THRESHOLD = 0.2;
//        LineCandidate::MAX_DISTANCE = 2;
//        FasTextOptions options;
//            erStatOptions.minThreshold = v;
//            erStatOptions.nonMaxSuppression = false;
////            erStatOptions.minProbabilityThresholdNM1 = (float) 0;
//            erStatOptions.minProbabilityNM1 = (float) 0;
//            erStatOptions.minProbabilityNM2 = (float) 0;
//        options.channels = ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_GRADIENT;//ImageUtils::CHANNELS_LUMINANCE;| ImageUtils::CHANNELS_Inverted
//        options.channels = ImageUtils::CHANNELS_LUMINANCE| ImageUtils::CHANNELS_Inverted;//ImageUtils::CHANNELS_LUMINANCE;
//        options.minThreshold = 16;
//        options.minProbabilityNM2 = (float) 0;
//        options.minProbabilityNM1 = 0;
//        options.nonMaxSuppression = true;
//        options.adaptiveThreshold = true;
//        options.adaptiveValue = 0.6;
//        PCharProcess er(
//                new ChannelWrapper<PCharacterCandidate>(
//                        new ScaleCharProcess(new FasTextDetector1C(options), 1.6), options.channels)
//        );
//        PCharProcess er(
//                new ScaleCharProcess(new FasTextDetector3C(options), 1.6)
//        );
//        PCharProcess charDetector(
//                new ScaleCharProcess(new ChannelWrapper<PCharacterCandidate>(
//                        new ErStatDetector(options), options.channels), 1.6)
//        );
        ErStatOptions options = ErStatOptions::getSpeed();
//        options.minThreshold = v;
//        options.minProbabilityNM1 = 0;
//        options.minProbabilityNM2 = 0;
//        options.channels =
//                ImageUtils::CHANNELS_GRADIENT | ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_Inverted;
//        options.channels =
//                ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_Inverted;
          options.channels =
                ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted;
        PCharProcess charDetector(
                new ErStatDetector(options)
        );
//        FasTextOptions fastOptions;
//        fastOptions.minProbabilityNM1 = 0.4;
//        PCharProcess charDetector(
//                new ScaleCharProcess(new FasTextDetector1C(fastOptions))
//        );
//        charDetector = new ScaleCharProcess(charDetector);
//        charDetector = new ChannelWrapper<PCharacterCandidate>(charDetector, options.channels);

        PLineProcess line = new CustomLineDetector(charDetector);
//        PLineProcess line = new NMCharLineDetector(options);
        line = new ChannelWrapper<PTextLine>(line, options.channels);
//        line = new ScaleLineProcess(line);
//        charDetector = new ScaleCharProcess(charDetector);
//        LineCharacterDetector *lineDetector = new LineCharacterDetector(
//                new ChannelWrapper<PTextLine>(new CustomLineDetector(charDetector), options.channels));
//        LineCharacterDetector *lineDetector = new LineCharacterDetector(new CustomLineDetector(charDetector));
//        LineCharacterDetector *lineDetector = new LineCharacterDetector(new ChannelWrapper<PTextLine>(line, options.channels));
//        LineCharacterDetector* lineDetector = new LineCharacterDetector(new ScaleLineProcess(new CustomLineDetector(charDetector)));

//        line = new TesseractSingleCharClassifier(line);
//        line = new SVMSingleCharClassifier(line, "/home/fab/dev/cpp/uic/models/character_classifier_test.xml");
//        line = new ComboOCR(line);
//        PCharProcess result = new LineCharacterDetector(line);

        PUicProcess uic = new SimpleUicDetector(line, new ComboOCR());
        PCharProcess result = new UicCharacterDetector(uic);
//        charDetector = new ChannelWrapper<PCharacterCandidate>(charDetector, options.channels);


        UicDebugHelper::imagePath = "/home/fab/dev/TEX/img/chars/ocr_ts_recall";
//        report.add(v, evaluator.evaluateCharDetector(lineDetector));
        report.add(v, evaluator.evaluateCharDetector(result));
        cout << "Value: " << v << endl;
        cout << options.toString();
        report.toCSV();
    }
    report.toCSV();
    return 0;
}