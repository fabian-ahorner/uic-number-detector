//
// Created by fab on 30.01.17.
//

#ifndef UIC_RINGBUFFER_H
#define UIC_RINGBUFFER_H

namespace std {
    template<class T>
    class RingBuffer {
    private:
        T *buffer;
        const int size;
        int shift = 0;
        int elemCnt = 0;
    public:
        RingBuffer(int size) : buffer(new T[size]), size(size) {
        }

        ~RingBuffer() {
            delete[] buffer;
        }

        inline void push(T elem) {
            shift = (shift + 1) % size;
            buffer[shift] = elem;
            if (elemCnt < size)
                elemCnt++;
        }

        inline void pop() {
            pop(1);
        }

        inline void pop(int cnt) {
            if (elemCnt >= cnt) {
                shift = (shift + size - cnt) % size;
                elemCnt -= cnt;
            }
        }

        inline void clear() {
            elemCnt = 0;
        }

        inline T operator[](long index) {
            return buffer[(this->shift - index + size) % size];
        }

        inline bool has(int index) {
            return index > 0 && index < elemCnt;
        }
    };
}


#endif //UIC_RINGBUFFER_H
