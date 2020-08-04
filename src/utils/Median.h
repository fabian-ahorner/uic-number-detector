//
// Created by fab on 18.06.17.
//

#ifndef UIC_MEDIAN_H
#define UIC_MEDIAN_H

#include "vector"
#include <algorithm>

namespace std {
    class Median {
        vector<double> values;
    public:
        inline void add(double v) {
            values.push_back(v);
        }

        inline double get() {
            std::sort(values.begin(), values.end());
            int middle = (int) (values.size() / 2);
            if (values.size() % 2 == 0) {
                double v1 = values[middle - 1];
                double v2 = values[middle];
                return v1 / 2 + v2 / 2;
            } else {
                return values[middle];
            }
        }
    };

}

#endif //UIC_MEDIAN_H
