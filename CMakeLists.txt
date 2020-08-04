cmake_minimum_required(VERSION 3.5)
project(uic)
find_package(OpenCV REQUIRED)
#find_package(Tesseract REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#include_directories(${Tesseract_INCLUDE_DIRS})

#include_directories(include)
#file(GLOB SOURCE_FILES "src/*.cpp")
include_directories(src/cv)
include_directories(src/char)
include_directories(src/char/erstat)
include_directories(src/char/fastext)
include_directories(src/line)
include_directories(src/ocr)
include_directories(src/ui)
include_directories(src/uic)
include_directories(src/utils)
include_directories(src)

set(SOURCE_FILES src/main.cpp
        src/Cli.cpp src/Cli.h
        # UI
        src/ui/ItemSelector.h #
        src/ui/UicDetectorSelector.cpp src/ui/UicDetectorSelector.h #
        src/ui/UicSelector.cpp src/ui/UicSelector.h #
        src/ui/UicWindow.cpp src/ui/UicWindow.h #

        # Utils
        src/utils/IOUtils.cpp src/utils/IOUtils.h #
        # src/utils/MathUtils.cpp src/utils/MathUtils.h #
        src/utils/ParallelUtils.cpp #
        src/utils/TimeLogger.cpp src/utils/TimeLogger.h #
        src/utils/TimeUtils.cpp src/utils/TimeUtils.h #

        # CV
        src/cv/ImagePipeline.h #
        src/cv/ImageUtils.cpp src/cv/ImageUtils.h #

        # CV / text
        src/char/CharacterDetector.h #
        src/char/CharacterCandidate.cpp src/char/CharacterCandidate.h #
        src/char/ChannelCharacterDetector.h #
        src/char/CharMerger.cpp src/char/CharMerger.h #
        src/char/erstat/ERStatDetector.h #
        src/char/erstat/ERStatCandidate.h src/char/MergedCandidate.h
        src/char/erstat/NMModelTrainer.cpp src/char/erstat/NMModelTrainer.h #
        src/char/fastext/FastCandidate.h #
        src/char/fastext/FastCharacter.h #
        src/char/fastext/FasTextDetector.h #
        src/char/fastext/FastPoint.h #
        src/char/fastext/FastPointDetector.h #
        src/char/fastext/FastPointSegmenter.h #
        src/char/fastext/NeighbourVisitor.cpp src/char/fastext/NeighbourVisitor.h #
        src/char/fastext/FastPointFactory.h #
        src/line/CustomLineDetector.cpp src/line/CustomLineDetector.h #
        src/line/LineCandidate.cpp src/line/LineCandidate.h #
        src/line/LineDetector.cpp src/line/LineDetector.h #
        src/line/NMLineDetector.cpp src/line/NMLineDetector.h #
        src/line/TextLine.cpp src/line/TextLine.h #
        src/ocr/CharClassifier.cpp src/ocr/CharClassifier.h #
        src/ocr/TextDetector.cpp src/ocr/TextDetector.h #
        src/ocr/OCRTesseract.cpp src/ocr/OCRTesseract.h #

        # CV / uic
        src/uic/UicDataset.cpp src/uic/UicDataset.h #
        src/uic/UicDebugHelper.cpp src/uic/UicDebugHelper.h #
        src/uic/UicDetector.cpp src/uic/UicDetector.h #
        src/uic/UicDetector.cpp src/uic/UicDetector.h #
        src/uic/UicEvaluator.cpp src/uic/UicEvaluator.h #
        src/utils/RingBuffer.h #
        src/line/ChannelLineDetector.h src/uic/DetectorEvaluator.cpp src/uic/DetectorEvaluator.h src/line/LineCharacterDetector.h src/char/fastext/FasTextOptions.h src/char/ScaledCharacter.h src/utils/DataCollector.h src/cv/ChannelWrapper.h src/cv/ScaleWrapper.h src/char/CharScalePiepline.h src/line/ScaleLineProcess.h src/uic/ScaleUicProcess.h src/uic/ChannelUicProcess.h src/line/NMCharLineDetector.h src/uic/UicCharDetector.h src/Cli.cpp src/Cli.h src/uic/UicUtils.h src/char/DistanceTransform.cpp src/char/DistanceTransform.h src/uic/FeedbackLoopProcess.cpp src/uic/FeedbackLoopProcess.h src/utils/Median.h src/char/MovedCharacter.h src/char/MovedCharacter.cpp src/uic/UicCombinerProcess.h src/test/CharDetectorEvaluator.cpp src/test/CharDetectorEvaluator.h src/char/CharGridFilter.cpp src/char/CharGridFilter.h src/char/GridFilterProcess.h src/char/CharDetectors.h src/results.h src/utils/UICDetectors.h)

add_executable(uic ${SOURCE_FILES})

target_link_libraries(uic ${OpenCV_LIBS} tesseract lept)