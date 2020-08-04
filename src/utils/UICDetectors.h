//
// Created by fab on 25.11.17.
//

#ifndef UIC_UICDETECTORS_H
#define UIC_UICDETECTORS_H

#include <UicDetector.h>
#include <../char/erstat/ERStatDetector.h>
#include <../line/CustomLineDetector.h>
#include <../line/ScaleLineProcess.h>
#include <../ocr/TextDetector.h>
#include <ChannelWrapper.h>


namespace cv {
    namespace uic {
        using namespace text;


        class UICDetectors {
            PUicProcess ER_Sp_L_SVM;
            PUicProcess ER_Sp_L;
            PUicProcess ER_Sp_L_Scale_SVM;
            PUicProcess ER_Sp_L_Scale;
            PUicProcess ER_Sp_RGB_Scale;
            PUicProcess ER_Sp_RGBD_Scale;


            UICDetectors() {
                ErStatOptions opErSp = ErStatOptions::getSpeed();
                ErStatOptions opErBa = ErStatOptions::getBalance();

                ER_Sp_L_SVM = new SimpleUicDetector(
                        new ChannelWrapper(new CustomLineDetector((new ErStatDetector(opErSp))),
                                           ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted),
                        new SVMSingleCharClassifier());
                ER_Sp_L = new SimpleUicDetector(
                        new ChannelWrapper(new CustomLineDetector((new ErStatDetector(opErSp))),
                                           ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted),
                        new ComboOCR());
                ER_Sp_L_Scale_SVM = new SimpleUicDetector(
                        new ChannelWrapper(new ScaleLineProcess(new CustomLineDetector((new ErStatDetector(opErSp)))),
                                           ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted),
                        new SVMSingleCharClassifier());
                ER_Sp_L_Scale = new SimpleUicDetector(
                        new ChannelWrapper(new ScaleLineProcess(new CustomLineDetector((new ErStatDetector(opErSp)))),
                                           ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted),
                        new ComboOCR());
                ER_Sp_RGB_Scale = new SimpleUicDetector(
                        new ChannelWrapper(new ScaleLineProcess(new CustomLineDetector((new ErStatDetector(opErSp)))),
                                           ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_Inverted),
                        new ComboOCR());
                ER_Sp_RGBD_Scale = new SimpleUicDetector(
                        new ChannelWrapper(new ScaleLineProcess(new CustomLineDetector((new ErStatDetector(opErSp)))),
                                           ImageUtils::CHANNELS_ALL),
                        new ComboOCR());
//
                ER_Sp_RGB_Scale = new SimpleUicDetector(
                        new ChannelWrapper(new ScaleLineProcess(new CustomLineDetector((new ErStatDetector(opErSp)))),
                                           ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_Inverted),
                        new ComboOCR());
////    ErStatOptions options = ErStatOptions::getSpeed();
//                FasTextOptions options = FasTextOptions::getRecall();
////    options.minProbabilityNM1=0.5;
////    options.minProbabilityNM2=0.5;
//                options.nonMaxSuppression = true;
////    options.channels = ImageUtils::CHANNELS_ALL;
////    options.channels = ImageUtils::CHANNELS_RGB  | ImageUtils::CHANNELS_Inverted;
////    options.channels = ImageUtils::CHANNELS_LUMINANCE | ImageUtils::CHANNELS_Inverted;
////    PCharProcess charDetector(new ErStatDetector(options));
////    PCharProcess charDetector(new FasTextDetector1C(options));
//                PCharProcess charDetector(new FasTextDetector3C(options));
//
//                PLineProcess line = new CustomLineDetector(charDetector);
////    line = new ChannelWrapper<PTextLine>(line, options.channels);
//                line = new ScaleLineProcess(line);
//
////    PUicProcess uic = new SimpleUicDetector(line, new TesseractSingleCharClassifier());
////    PUicProcess uic = new SimpleUicDetector(line, new ComboOCR());
//                PUicProcess uic = new SimpleUicDetector(line, new SVMSingleCharClassifier());
            }
        }
    }
}
#endif //UIC_UICDETECTORS_H
