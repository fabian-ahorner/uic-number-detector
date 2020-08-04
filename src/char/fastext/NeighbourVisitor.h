//
// Created by fab on 27.11.16.
//

#ifndef UIC_NEIGHBOURVISITOR_H
#define UIC_NEIGHBOURVISITOR_H

#include <opencv2/core/core.hpp>
#include <array>

using namespace std;
namespace cv {
    template<class Type>
    class NeighbourVisitor {
//        template<class Type>
        int referencCount = 1;

        Type const *start;
        Type const *end;
        int step;
    public:
        const Point *coordinates;
        const int *indices;
//        Mat &img;
        const int size;
        const int cols;

        NeighbourVisitor(const Mat &img, int size, const Point *coordinates, const int *indices, int step)
                : coordinates(coordinates), size(size), indices(indices), cols(img.cols) {
            start = &img.at<Type>(0, 0);
            end = &img.at<Type>(img.rows - 1, img.cols - 1);
            this->step = step;
        }

        NeighbourVisitor(const NeighbourVisitor &other)
                : coordinates(copy(other.coordinates, other.size)), size(other.size),
                  indices(copy(other.indices, other.size)),
                  cols(other.cols), start(other.start), end(other.end), step(other.step) {
            this->step = step;
        }

        static inline int *copy(const int *indices, int size) {
            int *copy = new int[size];
            for (int i = 0; i < size; i++) {
                copy[i] = indices[i];
            }
            return copy;
        }

        static inline Point *copy(const Point *coordinates, int size) {
            Point *copy = new Point[size];
            for (int i = 0; i < size; i++) {
                copy[i] = coordinates[i];
            }
            return copy;
        }

        ~NeighbourVisitor() {
            delete indices;
            delete coordinates;
        }

        bool isValid(Type *ptr);

        Point getPoint(const Type *ptr);

        Type *ptr(int row, int col, Mat &mat);

        const Type *ptr(int row, int col, const Mat &mat);

        /**
         * Returns a ptr to the neighbour or null if the neighbour is not inside the image
         * @param ptr must be a valid ptr to a lovation
         * @param index
         * @return ptr or null
         */
        inline Type *neighbour(Type *ptr, int index) const {
            Type *neighbour = ptr + indices[index];
            const Point &moveBy = coordinates[index];
            if (neighbour < start || neighbour > end)
                return nullptr;
            if (moveBy.x != 0) {
                unsigned long pos = (ptr - start) % step;
                if (pos == 0 || pos == cols)
                    return nullptr;
            }
            return neighbour;
        }
    };

    const vector<Point> NeighbourVisitor_CIRCLE8 = {
            Point(1, 1),
            Point(1, 0),
            Point(1, -1),
            Point(0, -1),
            Point(-1, -1),
            Point(-1, 0),
            Point(-1, 1),
            Point(0, 1)
    };
    const vector<Point> NeighbourVisitor_CIRCLE4 = {
            Point(1, 0),
            Point(0, -1),
            Point(-1, 0),
            Point(0, 1)
    };
    const vector<Point> NeighbourVisitor_CIRCLE16 = {
            Point(0, 2),
            Point(1, 2),
            Point(2, 2),
            Point(2, 1),
            Point(2, 0),
            Point(2, -1),
            Point(2, -2),
            Point(1, -2),
            Point(0, -2),
            Point(-1, -2),
            Point(-2, -2),
            Point(-2, -1),
            Point(-2, 0),
            Point(-2, 1),
            Point(-2, 2),
            Point(-1, 2)
    };
    const vector<Point> NeighbourVisitor_CIRCLE12 = {
            Point(0, 2),
            Point(1, 2),
            Point(2, 1),
            Point(2, 0),
            Point(2, -1),
            Point(1, -2),
            Point(0, -2),
            Point(-1, -2),
            Point(-2, -1),
            Point(-2, 0),
            Point(-2, 1),
            Point(-1, 2)
    };
    const vector<Point> NeighbourVisitor_CIRCLE12_CONNECTIONS = {
            Point(0, 1),
            Point(1, 1),
            Point(1, 1),
            Point(1, 0),
            Point(1, -1),
            Point(1, -1),
            Point(0, -1),
            Point(-1, -1),
            Point(-1, -1),
            Point(-1, 0),
            Point(-1, 1),
            Point(-1, 1)
    };

    template<class T>
    const NeighbourVisitor<T> createNeighbourVisitor(const Mat &img, const vector<Point> &coordinates) {
        int size = (int) coordinates.size();
        int *aIndices = new int[size];
        Point *aCoordinates = new Point[size];

        int step = (int) img.step[0] / (int) img.step[1];
        for (int i = 0; i < size; i++) {
            Point point = coordinates[i];
            aCoordinates[i] = point;
            aIndices[i] = point.x + point.y * step;
        }
        return NeighbourVisitor<T>(img, size, aCoordinates, aIndices, step);
    };

    template<class T>
    NeighbourVisitor<T> createNeighbourVisitor(Mat &img, const vector<Point> &coordinates) {
        int size = (int) coordinates.size();
        int *aIndices = new int[size];
        Point *aCoordinates = new Point[size];

        int step = (int) img.step[0] / (int) img.step[1];
        for (int i = 0; i < size; i++) {
            const Point &point = coordinates[i];
            aCoordinates[i] = point;
            aIndices[i] = point.x + point.y * step;
        }
        return NeighbourVisitor<T>(img, size, aCoordinates, aIndices, step);
    };

    template<class Type>
    bool NeighbourVisitor<Type>::isValid(Type *ptr) {
        if (ptr < start || ptr > end)
            return false;
        unsigned long pos = ptr - start;
        return pos % step < cols;
    }

    template<class Type>
    Point NeighbourVisitor<Type>::getPoint(const Type *ptr) {
        unsigned long pos = ptr - start;

        int x = (int) (pos % step);
        int y = (int) (pos / step);
        return Point(x, y);
    }

    template<class Type>
    Type *NeighbourVisitor<Type>::ptr(int row, int col, Mat &mat) {
        return &mat.at<Type>(row, col);
    }

    template<class Type>
    const Type *NeighbourVisitor<Type>::ptr(int row, int col, const Mat &mat) {
        return &mat.at<Type>(row, col);
    }
}


#endif //UIC_NEIGHBOURVISITOR_H
