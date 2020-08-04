//
// Created by fab on 28.11.16.
//

#ifndef UIC_TIMELOGGER_H
#define UIC_TIMELOGGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "TimeUtils.h"

using namespace std;
namespace std {
    struct TimeLogPoint {
        TimeLogPoint(long long int time, string tag);

        TimeLogPoint(string tag);

        TimeLogPoint();

        long long int time = 0;
        long long int totalDuration = 0;
        int count = 0;
        string tag;

        inline long long int getAvg() {
            return 0;
//            return count == 0 ? 0 : totalDuration / count;
        }

        bool operator<(const TimeLogPoint &other) const {
            return (totalDuration > other.totalDuration);
        }
    };

    class TimeLogger {
        static vector<TimeLogPoint *> timestamps;
        static unordered_map<string, TimeLogPoint> stats;
    public:
        static int logLevel;


        static void logStart(string tag);

        static void logEnd(string tag);

        static void log(string msg);

        static void log(const char *__restrict __format, ...);

        static void logF(const char *__restrict __format, ...);

        static inline void clear() {
            timestamps.clear();
            stats.clear();
        }

        static void print(string tag);

        static void printAll();
    };
}


#endif //UIC_TIMELOGGER_H
