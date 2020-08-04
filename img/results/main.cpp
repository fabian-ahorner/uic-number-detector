#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

tesseract::TessBaseAPI *getTesseract() {
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    int oemode = 3;
    int psmode = 6;

    if (api->Init(NULL, "eng", (tesseract::OcrEngineMode) oemode)) {
        printf("OCRTesseract: Could not initialize tesseract.");
        throw 1;
    }
    //cout << "OCRTesseract: tesseract version " << tess.Version() << endl;

    api->SetVariable("save_best_choices", "T");
    api->SetVariable("tessedit_char_whitelist", "0123456789");


    tesseract::PageSegMode pagesegmode = (tesseract::PageSegMode) psmode;
    api->SetPageSegMode(pagesegmode);
    return api;
}

int main() {

    printf("Loaded\r\n");
    Pix *pImage = pixRead("/home/fab/dev/cpp/uic/img/results/line1.png");
//
    // Open input image with leptonica library
//    // Get OCR result
    char *outText;
    Pix image = *pImage;
    tesseract::TessBaseAPI *api = getTesseract();
    for (int i = 0; i < 10; ++i) {
//        api->Recognize(0);
//        tesseract::TessBaseAPI api = *pApi;
        api->SetImage(&image);
        outText = api->GetUTF8Text();
        api->ClearAdaptiveClassifier();
        printf("%s\r\n", outText);
        delete[] outText;
//        api->Clear();
//        delete image;

//        image = pixRead("/home/fab/dev/cpp/uic/img/results/line2.png");
//        api->SetImage(image);
//        api->Recognize(0);
//        outText = api->GetUTF8Text();
//        printf("%s\r\n", outText);
//        delete [] outText;
    }
    delete api;
    delete pImage;
//
//    // Destroy used object and release memory
//    api->End();
//    delete [] outText;
//    pixDestroy(&image);

    return 0;
}
