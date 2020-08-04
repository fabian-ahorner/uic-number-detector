//
// Created by fab on 20.10.16.
//

#ifndef UIC_PARALLELUTILS_H
#define UIC_PARALLELUTILS_H

#include <functional>
#include "opencv2/core/core.hpp"

using namespace cv;

class ParallelUtilBody : public cv::ParallelLoopBody {
private:
    std::function<void(const cv::Range)> body;
//    ParallelUtilBody &operator=(const ParallelUtilBody &); // to quiet MSVC

public:
    ParallelUtilBody(std::function<void(const cv::Range)> body)
            : body(body) {
    }

    inline void operator()(const cv::Range &range) const {
        body(range);
    }
};

inline void parallel_for(const Range &range, std::function<void(const cv::Range)> body) {
//    body(range);
    cv::parallel_for_(range, ParallelUtilBody(body));
}

inline void parallel_for(int from, int to, std::function<void(const cv::Range)> body) {
    parallel_for(Range(from, to), body);
}


#endif //UIC_PARALLELUTILS_H
