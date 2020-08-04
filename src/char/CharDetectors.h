//
// Created by fab on 16.09.17.
//

#ifndef UIC_CHARDETECTORS_H
#define UIC_CHARDETECTORS_H

#include <ChannelWrapper.h>
#include <erstat/ERStatDetector.h>
#include "CharacterDetector.h"
#include "CharScalePiepline.h"

using namespace std;
namespace cv {
    namespace text {
        namespace CharDetectors {

            static PCharProcess ER_SPEED(
                    new ScaleCharProcess(new ChannelWrapper<PCharacterCandidate>(
                            new ErStatDetector(ErStatOptions::getSpeed()),
                            ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_Inverted), 1.6)
            );

            static PCharProcess ER_RECALL(
                    new ScaleCharProcess(new ChannelWrapper<PCharacterCandidate>(
                            new ErStatDetector(ErStatOptions::getBalance()),
                            ImageUtils::CHANNELS_RGB | ImageUtils::CHANNELS_GRADIENT | ImageUtils::CHANNELS_Inverted),
                                         1.6)
            );
        };
    }
}
#endif //UIC_CHARDETECTORS_H
