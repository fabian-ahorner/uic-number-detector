//
// Created by fab on 07.03.17.
//

#ifndef UIC_DATACOLLECTOR_H
#define UIC_DATACOLLECTOR_H

#include <vector>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

template<class T>
class DataCollector {
    Ptr <vector<T>> data = new vector<T>();
public:
    inline void add(const T& element){
        data->push_back(element);
    }
    inline void add(const Ptr <vector<T>>& data){
        this->data->insert(this->data->end(), data->begin(), data->end());
    }
    inline void add(const vector<T>& data){
        this->data->insert(this->data->end(), data.begin(), data.end());
    }
    inline vector<T> collect(){
        return *data;
    }
    inline Ptr <vector<T>> dataPtr(){
        return data;
    }
};

#endif //UIC_DATACOLLECTOR_H
