#include "KMP.h"
#include "../Utils/IO.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// =====================================================
// LPS(Longest Prefix Suffix)
// =====================================================

vector<int> buildLPS(const string& pattern) {

    vector<int> lps(pattern.size(), 0);

    int len = 0;
    int i = 1;

    while (i < (int)pattern.size()) {

        if (pattern[i] == pattern[len]) {

            len++;
            lps[i] = len;
            i++;
        }
        else {

            if (len != 0) {
                len = lps[len - 1];
            }
            else {
                lps[i] = 0;
                i++;
            }
        }
    }

    return lps;
}

// =====================================================
// KMP Exact Matching
// =====================================================

vector<int> KMPSearchAll(
    const string& genome,
    const string& shortRead
) {
    vector<int> positions;

    if (shortRead.empty() ||
        shortRead.size() > genome.size()) {

        return positions;
    }

    vector<int> lps =
        buildLPS(shortRead);

    int i = 0;
    int j = 0;

    while (i < (int)genome.size()) {

        if (genome[i] == shortRead[j]) {
            i++;
            j++;
        }

        if (j == (int)shortRead.size()) {

            positions.push_back(i - j);

            j = lps[j - 1];
        }
        else if (
            i < (int)genome.size() &&
            genome[i] != shortRead[j]
        ) {

            if (j != 0) {
                j = lps[j - 1];
            }
            else {
                i++;
            }
        }
    }

    return positions;
}

// =====================================================
// mismatch 개수 계산
// =====================================================

int countMismatchUpTo(
    const string& genome,
    int start,
    const string& shortRead,
    int allowedMismatch
) {
    int mismatch = 0;

    for (int i = 0;
         i < (int)shortRead.size();
         i++) {

        if (genome[start + i] != shortRead[i]) {

            mismatch++;

            if (mismatch > allowedMismatch) {
                return mismatch;
            }
        }
    }

    return mismatch;
}

// =====================================================
// approximate matching
// =====================================================

info findBestApproximateMatch(
    const string& genome,
    const string& shortRead,
    int allowedMismatch
) {
    info result;

    result.position = -1;
    result.mismatch = -1;
    result.ambiguous = false;

    if (shortRead.size() > genome.size()) {
        return result;
    }

    int bestMismatch =
        allowedMismatch + 1;

    int bestCount = 0;

    for (size_t start = 0;
         start <= genome.size() - shortRead.size();
         start++) {

        int mismatch =
            countMismatchUpTo(
                genome,
                start,
                shortRead,
                allowedMismatch
            );

        if (mismatch <= allowedMismatch) {

            if (mismatch < bestMismatch) {

                bestMismatch = mismatch;

                result.position =
                    static_cast<int>(start);

                result.mismatch =
                    mismatch;

                bestCount = 1;

                result.ambiguous = false;
            }
            else if (mismatch == bestMismatch) {

                bestCount++;

                result.ambiguous = true;
            }
        }
    }

    return result;
}

// =====================================================
// 전체 mapping
// =====================================================

vector<info> KMPMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
) {
    vector<info> results;

    results.reserve(shortReads.size());

    for (const string& shortRead : shortReads) {

        vector<int> exactMatches =
            KMPSearchAll(genome, shortRead);

        info result;

        result.position = -1;
        result.mismatch = -1;
        result.ambiguous = false;

        // exact match 존재
        if (!exactMatches.empty()) {

            result.position =
                exactMatches[0];

            result.mismatch = 0;

            if (exactMatches.size() >= 2) {
                result.ambiguous = true;
            }
        }
        else {

            result =
                findBestApproximateMatch(
                    genome,
                    shortRead,
                    allowedMismatch
                );
        }

        results.push_back(result);
    }

    return results;
}

// =====================================================
// 실행 함수
// =====================================================

void runKMPMappingFile(const Config& cfg) {

    int allowedMismatch =
        cfg.allowedMismatch;

    if (allowedMismatch < 0) {
        throw runtime_error(
            "허용 mismatch 수는 0 이상의 정수여야 합니다."
        );
    }

    string genome =
        loadGenome("0_DNA.txt");

    vector<string> shortReads =
        loadShortReads("1_ShortReads.txt");

    vector<info> results =
        KMPMapping(
            genome,
            shortReads,
            allowedMismatch
        );

    saveResults(
        "6_KMPMapping_result.txt",
        results
    );

    cout << "6_KMPMapping_result.txt 저장 완료\n";
}