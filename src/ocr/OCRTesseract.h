//
// Created by fab on 11.02.17.
//

#ifndef UIC_OCRTESSERACT_H
#define UIC_OCRTESSERACT_H

#include "opencv2/core.hpp"
#include "opencv2/text/ocr.hpp"

/** @brief OCRTesseract class provides an interface with the tesseract-ocr API (v3.02.02) in C++.

Notice that it is compiled only when tesseract-ocr is correctly installed.

@note
   -   (C++) An example of OCRTesseract recognition combined with scene text detection can be found
        at the end_to_end_recognition demo:
        <https://github.com/opencv/opencv_contrib/blob/master/modules/text/samples/end_to_end_recognition.cpp>
    -   (C++) Another example of OCRTesseract recognition combined with scene text detection can be
        found at the webcam_demo:
        <https://github.com/opencv/opencv_contrib/blob/master/modules/text/samples/webcam_demo.cpp>
 */
namespace cv {
    namespace text {
        class OCRTesseract2 {
        public:
/** @brief Recognize text using the tesseract-ocr API.

Takes image on input and returns recognized text in the output_text parameter. Optionally
provides also the Rects for individual text elements found (e.g. words), and the list of those
text elements with their confidence values.

@param image Input image CV_8UC1 or CV_8UC3
@param output_text Output text of the tesseract-ocr.
@param component_rects If provided the method will output a list of Rects for the individual
text elements found (e.g. words or text lines).
@param component_texts If provided the method will output a list of text strings for the
recognition of individual text elements found (e.g. words or text lines).
@param component_confidences If provided the method will output a list of confidence values
for the recognition of individual text elements found (e.g. words or text lines).
@param component_level OCR_LEVEL_WORD (by default), or OCR_LEVEL_TEXT_LINE.
 */
            virtual void run(Mat &image, std::string &output_text, std::vector<Rect> *component_rects = NULL,
                             std::vector<std::string> *component_texts = NULL,
                             std::vector<float> *component_confidences = NULL,
                             int component_level = 0);

            virtual void run(Mat &image, Mat &mask, std::string &output_text, std::vector<Rect> *component_rects = NULL,
                             std::vector<std::string> *component_texts = NULL,
                             std::vector<float> *component_confidences = NULL,
                             int component_level = 0);

// aliases for scripting
            CV_WRAP String run(InputArray image, int min_confidence, int component_level = 0);

            CV_WRAP String run(InputArray image, InputArray mask, int min_confidence, int component_level = 0);

            CV_WRAP virtual void setWhiteList(const String &char_whitelist) = 0;
            CV_WRAP virtual void setVariable(const String &name,const String value) = 0;
            CV_WRAP virtual void clearAdaptiveClassifier() = 0;
            CV_WRAP virtual void disableAdaptiveClassifier() = 0;


/** @brief Creates an instance of the OCRTesseract class. Initializes Tesseract.

@param datapath the name of the parent directory of tessdata ended with "/", or NULL to use the
system's default directory.
@param language an ISO 639-3 code or NULL will default to "eng".
@param char_whitelist specifies the list of characters used for recognition. NULL defaults to
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ".
@param oem tesseract-ocr offers different OCR Engine Modes (OEM), by deffault
tesseract::OEM_DEFAULT is used. See the tesseract-ocr API documentation for other possible
values.
@param psmode tesseract-ocr offers different Page Segmentation Modes (PSM) tesseract::PSM_AUTO
(fully automatic layout analysis) is used. See the tesseract-ocr API documentation for other
possible values.
 */
            CV_WRAP static Ptr<OCRTesseract2> create(const char *datapath = NULL, const char *language = NULL,
                                                     const char *char_whitelist = NULL, int oem = 3, int psmode = 3);
        };

    }
}
#endif //UIC_OCRTESSERACT_H
