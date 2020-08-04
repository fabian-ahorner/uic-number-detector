//
// Created by fab on 11.01.17.
//

#include <ScaleLineProcess.h>
#include <ScaleUicProcess.h>
#include <ChannelUicProcess.h>
#include <NMCharLineDetector.h>
#include <FeedbackLoopProcess.h>
#include "UicDetectorSelector.h"
#include "../char/fastext/FasTextDetector.h"
#include "../char/CharScalePiepline.h"
#include "../line/NMLineDetector.h"
#include "../line/CustomLineDetector.h"
#include "../char/ChannelCharacterDetector.h"

static const string CLASSIFIER_NM1 = "/home/fab/dev/cpp/uic/models/trained_classifierNM1.xml";
static const string CLASSIFIER_NM2 = "/home/fab/dev/cpp/uic/models/trained_classifierNM2.xml";

UicDetectorSelector::UicDetectorSelector() : UicDetectorSelector(createDetectors()) {
}

UicDetectorSelector::UicDetectorSelector(vector<PUicProcess> detectors) : ItemSelector(detectors) {
}

vector<PUicProcess> UicDetectorSelector::createDetectors() {
    int fastChannelMode = ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_LUMINANCE;//
    int erChannelMode = ImageUtils::CHANNELS_ALL;

    FasTextOptions fastTextOptions;
    ErStatOptions erStatOptions;


    PCharProcess erStatDetector(new ErStatDetector(erStatOptions));
    PCharProcess fastDetector3C(new ScaleCharProcess(new FasTextDetector3C(fastTextOptions)));
    PCharProcess fastDetector1C(new ScaleCharProcess(new FasTextDetector1C(fastTextOptions)));
    PCharProcess erChannelStatDetector(
            new ChannelWrapper<PCharacterCandidate>(erStatDetector, erChannelMode));
    PCharProcess fastChannelDetector(
            new ChannelCharacterDetector(fastDetector1C, fastChannelMode));

    Ptr<LineDetector> erSingleNM(new NMLineDetector(erStatDetector));
    Ptr<LineDetector> erSingleCustom(new CustomLineDetector(erStatDetector));
    Ptr<LineDetector> erNM(new NMLineDetector(erChannelStatDetector));
    Ptr<LineDetector> erCustom(new CustomLineDetector(erChannelStatDetector));
    Ptr<LineDetector> fast3CCustom(new CustomLineDetector(fastDetector3C));
    Ptr<LineDetector> fast1CCustom(new CustomLineDetector(fastChannelDetector));

    Ptr<TextDetector> ocrSVM = new SVMSingleCharClassifier(
            "/home/fab/dev/cpp/uic/models/character_classifier_test.xml");
    Ptr<TextDetector> ocrTesseractSingle = new TesseractSingleCharClassifier(true);
    Ptr<TextDetector> ocrCombo = new ComboOCR();

    PUicProcess fastSVM(
            new SimpleUicDetector(fast3CCustom, ocrSVM));
    PUicProcess fastTessSingle(
            new SimpleUicDetector(fast3CCustom, ocrTesseractSingle));
    PUicProcess fast1CTessSingle(
            new SimpleUicDetector(fast1CCustom, ocrTesseractSingle));
    PUicProcess fastTessLine(
            new SimpleUicDetector(fast3CCustom, ocrTesseractSingle));
    PUicProcess erTessSingle(
            new SimpleUicDetector(erCustom, ocrTesseractSingle));
    PUicProcess erNMTessSingle(
            new SimpleUicDetector(erNM, ocrTesseractSingle));
    PUicProcess erNMChannels(
            new ChannelUicDetector(erSingleNM, ocrTesseractSingle, erChannelMode));
    PUicProcess erChannels(
            new ChannelUicDetector(erSingleCustom, ocrTesseractSingle, erChannelMode));
    PUicProcess fastChannels(
            new ChannelUicDetector(new CustomLineDetector(fastDetector1C), ocrTesseractSingle,
                                   fastChannelMode));
    PUicProcess fastLineScaleUicChannel(
            new ChannelUicDetector(new ScaleLineProcess(new CustomLineDetector(new FasTextDetector1C(fastTextOptions))),
                                   ocrTesseractSingle,
                                   fastChannelMode));

    PUicProcess fastLineScaleChannel(
            new SimpleUicDetector(new ChannelWrapper<PTextLine>(
                    new ScaleLineProcess(new CustomLineDetector(new FasTextDetector1C(fastTextOptions))),
                    fastChannelMode),
                                  ocrTesseractSingle));

    PUicProcess fastBest(
            new ScaleUicProcess(new SimpleUicDetector(new ChannelWrapper<PTextLine>(
                    new CustomLineDetector(new FasTextDetector1C(fastTextOptions)),
                    fastChannelMode), ocrTesseractSingle)));
    PUicProcess erBest(new ScaleUicProcess(
            new SimpleUicDetector(new ChannelWrapper<PTextLine>(
                    new CustomLineDetector(erStatDetector),
                    erChannelMode), ocrTesseractSingle)));
    PUicProcess lastTested2(
            new ScaleUicProcess(new ChannelUicProcess(new SimpleUicDetector(
                    new CustomLineDetector(new FasTextDetector1C(fastTextOptions)),
                    ocrTesseractSingle), fastChannelMode)));

    FasTextOptions fast3Options;
//    fast3Options.nonMaxSuppression = true;
    fast3Options.minProbabilityNM1 = 0.1;
    fast3Options.minProbabilityNM2 = 0.5;
    fast3Options.minThreshold = 60;

    PUicProcess fast3(
            new ScaleUicProcess(new SimpleUicDetector(
                    new CustomLineDetector(new FasTextDetector3C(fast3Options)),
                    ocrTesseractSingle)));
    PUicProcess nm(new SimpleUicDetector(new ChannelWrapper<PTextLine>(
            new NMCharLineDetector(erStatOptions)),
                                         ocrTesseractSingle));

    ErStatOptions erStatOptionsMany;
    erStatOptionsMany.minThreshold = 1;
    erStatOptionsMany.minProbabilityNM2 = 0.8;
    erStatOptionsMany.minProbabilityNM1 = 0.9;
    ErStatOptions erStatOptionsFew;
    erStatOptionsFew.minThreshold = 16;
    erStatOptionsFew.minProbabilityNM2 = 0.8;
    erStatOptionsFew.minProbabilityNM1 = 0.9;


    PUicProcess primaryDetector(
            new SimpleUicDetector(new ChannelWrapper<PTextLine>(
                    new CustomLineDetector(new ErStatDetector(erStatOptionsFew)),
                    ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted), ocrCombo));

    Ptr<CharacterFilter> charFilter = Ptr<CharacterFilter>(
            new CharacterFilter(new ErStatDetector(ErStatOptions::getBalance())));
//    Ptr<CharacterFilter> charFilter = Ptr<CharacterFilter>(new CharacterFilter(new ErStatDetector(erStatOptionsMany)));
    PUicProcess secondaryDetector(new ScaleUicProcess(
            new SimpleUicDetector(new ChannelWrapper<PTextLine>(
                    new CustomLineDetector(charFilter),
                    ImageUtils::CHANNELS_ALL), ocrCombo)));


    PUicProcess feedbackLoop(new FeedbackLoopProcess(primaryDetector, secondaryDetector, charFilter));




    Ptr<CharacterFilter> balanceFilter = Ptr<CharacterFilter>(
            new CharacterFilter(new ErStatDetector(ErStatOptions::getSpeed())));
    PUicProcess erBalance(
            new SimpleUicDetector(new ScaleLineProcess(new ChannelWrapper<PTextLine>(
                    new CustomLineDetector(balanceFilter),
                    ImageUtils::CHANNELS_ALL)), ocrCombo));
    PUicProcess erFast(
            new SimpleUicDetector(new ChannelWrapper<PTextLine>(
                    new CustomLineDetector(new ErStatDetector(ErStatOptions::getSpeed())),
                    ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted), ocrSVM));
    PUicProcess feedbackLoop2(new FeedbackLoopProcess(erFast, erBalance, balanceFilter));

    PUicProcess erBalance2(
            new SimpleUicDetector(new ScaleLineProcess(new ChannelWrapper<PTextLine>(
                    new CustomLineDetector(new ErStatDetector(ErStatOptions::getSpeed())),
                    ImageUtils::CHANNELS_ALL)), ocrCombo));

    vector<PUicProcess> detectors = {feedbackLoop2, erBalance2, erFast, feedbackLoop, primaryDetector, secondaryDetector,
                                     fastBest, erBest, nm,
                                     erNMChannels, lastTested2, fastLineScaleChannel,
                                     fastLineScaleUicChannel, fastChannels, fastTessSingle, erTessSingle, erChannels,
                                     fastTessLine, fastSVM, erNMTessSingle, fast1CTessSingle
    };
    return detectors;
}

void UicDetectorSelector::onSelect(int index, PUicProcess detector) {
    ItemSelector::onSelect(index, detector);
    cout << "Detector: " << detector->toString() << endl;
}

PUicProcess UicDetectorSelector::getCurrentDetector() {
    return getCurrent();
}
