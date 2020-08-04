//
// Created by fab on 28.11.16.
//

#include "TimeLogger.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <cstdarg>

namespace std {
    int TimeLogger::logLevel = 0;
    vector<TimeLogPoint *> TimeLogger::timestamps;
    unordered_map<string, TimeLogPoint> TimeLogger::stats;

    void TimeLogger::logStart(string tag) {
        TimeLogPoint *point;
        if (!stats.count(tag)) {
            stats[tag] = TimeLogPoint(tag);
        }
        point = &stats[tag];
        if (point->time == 0) {
            point->time = TimeUtils::MiliseconsNow();
            timestamps.push_back(point);
            if (logLevel >= timestamps.size()) {
                cout << std::string(2 * (timestamps.size() - 1), ' ') << "{ " << tag << endl;
            }
        }
    }

    void TimeLogger::logEnd(string tag) {
        TimeLogPoint &point = *timestamps.back();
        unsigned long s = timestamps.size();
        if (tag == point.tag) {
            long long time = TimeUtils::MiliseconsNow();
            double duration = time - point.time;
            point.time = 0;
            point.totalDuration += duration;
            point.count++;

            if (logLevel >= timestamps.size()) {
//                cout << std::string(2 * (timestamps.size() - 1), ' ') << "} " << tag << ": " << duration << "/"
//                     << point.getAvg() << endl;
            }
            timestamps.pop_back();
        }
    }

    void TimeLogger::log(string msg) {
        if (logLevel >= timestamps.size() + 1) {
            cout << std::string(2 * (timestamps.size()), ' ') << ">>" << msg << endl;
        }
    }

    void TimeLogger::log(const char *__restrict __format, ...) {
        if (logLevel >= timestamps.size() + 1) {
            va_list arg;
            va_start (arg, __format);
            cout << std::string(2 * (timestamps.size()), ' ') + ">> ";
            vfprintf(stdout, __format, arg);
            va_end (arg);
            cout << endl;
        }
    }

    void TimeLogger::logF(const char *__format, ...) {
        va_list arg;
        va_start (arg, __format);
        cout << std::string(2 * (timestamps.size()), ' ') + ">> ";
        vfprintf(stdout, __format, arg);
        va_end (arg);
        cout << endl;
    }

    void TimeLogger::print(string tag) {
        if (stats.count(tag)) {
            printf("%30s:%5d %5d\r\n", tag.c_str(), (int) stats[tag].getAvg(), (int) stats[tag].totalDuration);
//            cout << tag << ": " << stats[tag].getAvg() << endl;
        }
    }

    void TimeLogger::printAll() {
        vector<TimeLogPoint> sorted;
        for (auto s:stats)
            sorted.push_back(s.second);
        std::sort(sorted.begin(), sorted.end());
        for (auto &s:sorted)
            printf("%5d \t %s\r\n",  (int) s.totalDuration, s.tag.c_str());
    }

    TimeLogPoint::TimeLogPoint(long long int time, string tag) : time(time), tag(tag) {}

    TimeLogPoint::TimeLogPoint(string tag) : tag(tag) {}

    TimeLogPoint::TimeLogPoint() {}

}
