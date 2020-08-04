//
// Created by fab on 11.04.17.
//

#ifndef UIC_UICUTILS_H
#define UIC_UICUTILS_H

#include <string>
#include <vector>
#include <opencv2/core/cvdef.h>

using namespace std;

namespace cv {
    namespace uic {
        class UicUtils {
            inline static size_t min(size_t x, size_t y, size_t z) {
                return x < y ? MIN(x, z) : MIN(y, z);
            }

        public:
            static inline bool isValid(string uic) {
                if (uic.length() == 12) {
                    int sum = 0;
                    for (int i = 0; i < 12; i++) {
                        int multiply = 2 - i % 2;
                        if (uic[i] < '0' || uic[i] > '9')
                            return false;
                        int value = multiply * (uic[i] - '0');
                        if (value > 9)
                            value -= 9;
                        sum += value;
                    }
                    return sum % 10 == 0;
                }
                return false;
            }

            static inline size_t editDistance(const string &A, const string &B) {
                size_t NA = A.size();
                size_t NB = B.size();

                vector<vector<size_t> > M(NA + 1, vector<size_t>(NB + 1));

                for (size_t a = 0; a <= NA; ++a)
                    M[a][0] = a;

                for (size_t b = 0; b <= NB; ++b)
                    M[0][b] = b;

                for (size_t a = 1; a <= NA; ++a)
                    for (size_t b = 1; b <= NB; ++b) {
                        size_t x = M[a - 1][b] + 1;
                        size_t y = M[a][b - 1] + 1;
                        size_t z = M[a - 1][b - 1] + (A[a - 1] == B[b - 1] ? 0 : 1);
                        M[a][b] = min(x, y, z);
                    }

                return M[A.size()][B.size()];
            }

            static inline string checkWhitelist(string uic, const vector<string> whitelist) {
                string bestMatchUic = uic;
                int bestMatchDistance = 13;
                bool isOnlyMatch = false;

                for (string candidate:whitelist) {
                    int distance = editDistance(uic, candidate);
                    if (distance < bestMatchDistance) {
                        bestMatchUic = candidate;
                        bestMatchDistance = distance;
                        isOnlyMatch = true;
                    } else if (distance == bestMatchDistance) {
                        isOnlyMatch = false;
                    }
                }
                if (isOnlyMatch)
                    return bestMatchUic;
                return uic;
            }

        };
    }
}
#endif //UIC_UICUTILS_H
