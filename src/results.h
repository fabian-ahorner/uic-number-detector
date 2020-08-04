//
// Created by fab on 18.11.17.
//

#ifndef UIC_RESULTS_H
#define UIC_RESULTS_H


#include <FeedbackLoopProcess.h>

void computeResults() {
    UicDatasetHelper dataset("/data", 0, 0.5);
    cout<< dataset.full.size()<<endl;

//    ErStatOptions options = ErStatOptions::getSpeed();
//    FasTextOptions options = FasTextOptions::getRecall();
////    options.minProbabilityNM1=0.5;
////    options.minProbabilityNM2=0.5;
//    options.nonMaxSuppression = true;
////    options.channels = ImageUtils::CHANNELS_ALL;
//    options.channels = ImageUtils::CHANNELS_LUMINANCE;
////    options.channels = ImageUtils::CHANNELS_RGB  | ImageUtils::CHANNELS_Inverted;
////    options.channels = ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted;
////    PCharProcess charDetector(new ErStatDetector(options));
//    PCharProcess charDetector(new FasTextDetector1C(options));
////    PCharProcess charDetector(new FasTextDetector3C(options));
//
//    PLineProcess line = new CustomLineDetector(charDetector);
//    line = new ChannelWrapper<PTextLine>(line, options.channels);
//    line = new ScaleLineProcess(line);
////
////    PUicProcess uic = new SimpleUicDetector(line, new TesseractSingleCharClassifier());
////    PUicProcess uic = new SimpleUicDetector(line, new ComboOCR());
//    PUicProcess uic = new SimpleUicDetector(line, new SVMSingleCharClassifier());

    ErStatOptions opErSp = ErStatOptions::getSpeed();
    ErStatOptions opErBa = ErStatOptions::getBalance();

    SimpleUicDetector *ER_Sp_L = new SimpleUicDetector(
                new ChannelWrapper<PTextLine>(new CustomLineDetector((new ErStatDetector(opErSp))),
                                   ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted), new ComboOCR());

    Ptr<CharacterFilter> charFilter = Ptr<CharacterFilter>(
            new CharacterFilter(new ErStatDetector(opErBa)));
//    Ptr<CharacterFilter> charFilter = Ptr<CharacterFilter>(new CharacterFilter(new ErStatDetector(erStatOptionsMany)));
    PUicProcess secondaryDetector(
            new SimpleUicDetector(new ChannelWrapper<PTextLine>(new ScaleLineProcess(
                    new CustomLineDetector(charFilter)),
                                                                ImageUtils::CHANNELS_RGB| ImageUtils::CHANNELS_Inverted), new ComboOCR()));


    PUicProcess uic(new FeedbackLoopProcess(ER_Sp_L, secondaryDetector, charFilter));


    //    const PUicResultVector &uics = uic->run();
        UicEvaluator::evaluateUicStrings(uic, dataset.test, UicEntry::ImageType::TRAIN);//)
}

#endif //UIC_RESULTS_H
