//
// Created by fab on 08.01.17.
//

#ifndef UIC_IMAGEPIPELINE_H
#define UIC_IMAGEPIPELINE_H

#include "../utils/TimeLogger.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

template<class Out>
class ImageProcess {
    Mat originalImage;
protected:
    virtual string getProcessName()=0;

public:
    virtual Out processImage(Mat original, Mat processed)=0;

    virtual string toString() {
        return getProcessName();
    }

    virtual inline void setOriginal(Mat img) {
        originalImage = img;
    }

    Out run(Mat img) {
        return processImage(img, img);
    }

    Out runWithLAB(Mat img) {
        Mat lab;
        cvtColor(img, lab, COLOR_BGR2Lab);
        return processImage(lab, img);
    }

    inline Mat getOriginal() {
        return originalImage;
    }
};

template<class In, class Out>
class ParentImageProcess : public ImageProcess<Out> {
protected:
public:
    Ptr<ImageProcess<In>> child;

    inline Ptr<ImageProcess<In>> getChild() {
        return child;
    }

    inline void setChild(Ptr<ImageProcess<In>> child) {
        this->child = child;
    }
};

template<class Out>
class WrapperImageProcess : public ParentImageProcess<Out,Out> {
};

template<class In, class Out>
class ImagePipeline : public ParentImageProcess<In, Out> {
//    Ptr<ImageProcess<In>> child;

public :
    virtual string getProcessName()=0;

    virtual Out process(Mat &original, Mat &processed, In in)=0;

    virtual inline Out processImage(Mat original, Mat processed) {
        In in;
//        ImageProcess<In> *childPipeline = dynamic_cast<ImageProcess<In> *>(child.get());
        in = ParentImageProcess<In,Out>::getChild()->processImage(original, processed);
        Out out = processStep(original, processed, in);
        return out;
    }

    virtual inline void setOriginal(Mat img) {
        ImageProcess<Out> *parent = dynamic_cast<ImageProcess<Out> *>(this);
//        ImageProcess<In> *childPipeline = dynamic_cast<ImageProcess<In> *>(child.get());
        ParentImageProcess<In,Out>::child->setOriginal(img);
    }

    Out processStep(Mat &img, Mat &processed, In in) {
        TimeLogger::logStart(toString());
        Out result = process(img, processed, in);
        TimeLogger::logEnd(toString());
        return result;
    }

//    Ptr<ImageProcess<In>> getChildPipeline() {
//        return child;
//    }
//
//    void setChildPipeline(Ptr<ImageProcess<In>> child) {
//        this->child = child;
//    }


    virtual string toString() {
        if(this->child.empty())
            return getProcessName();
        return this->child->toString() + " -> " + getProcessName();
    }
};

class PipelineStage1 : public ImageProcess<String> {
public:
    virtual String processImage(Mat original, Mat in)=0;

    virtual inline string getProcessName() {
        return "s1";
    }
};

class PipelineStage1A : public PipelineStage1 {
public:
    inline String processImage(Mat original, Mat in) {
        return "stage1A";
    }
};

class PipelineStage1B : public PipelineStage1 {
public:
    inline String processImage(Mat original, Mat in) {
        return "stage1B";
    }

    virtual inline string getProcessName() {
        return "s1b";
    }
};

class PipelineStage2 : public ImagePipeline<String, String> {
public :
    inline String process(Mat &original, Mat &img, String in) {
        return in + " -> stage2";
    }

    virtual inline string getProcessName() {
        return "s2";
    }
};

class PiplineTest {
public:
    static void testPipeline() {
        Ptr<PipelineStage1> stage1 = Ptr<PipelineStage1>(new PipelineStage1A());
        Ptr<PipelineStage2> stage2 = Ptr<PipelineStage2>(new PipelineStage2());
        stage2->setChild(stage1);

        Mat mat;
        String out = stage2->processImage(mat, mat);
        std::cout << stage2->toString() << std::endl;
        std::cout << "Out: " << out;
    }
};

#endif //UIC_IMAGEPIPELINE_H
