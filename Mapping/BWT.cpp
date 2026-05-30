#include "BWT.h"
#include "../Utils/IO.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// FM-index에서 사용할 문자 집합을 고정 인덱스로 바꾼다.
int charToIndex(char c) {
    if (c == '$') return 0;
    if (c == 'A') return 1;
    if (c == 'C') return 2;
    if (c == 'G') return 3;
    if (c == 'T') return 4;
    return -1;
}

struct FMIndex {
    string text;
    string bwt;
    vector<int> sa;
    vector<int> C;
    vector<vector<int>> occ;
};

struct MatchInfo {
    int position;
    int mismatch;
};


// MatchInfo vector를 info vector로 변경
vector<info> summarizeResults(
    const vector<vector<MatchInfo>>& matchesByRead
) {
    vector<info> results(matchesByRead.size());

    for (size_t i = 0; i < matchesByRead.size(); i++) {
        info result;
        result.position = -1;
        result.mismatch = -1;
        result.ambiguous = false;

        if (!matchesByRead[i].empty()) {
            result.position = matchesByRead[i][0].position;
            result.mismatch = matchesByRead[i][0].mismatch;
            result.ambiguous = matchesByRead[i].size() >= 2;
        }

        results[i] = result;
    }

    return results;
}

vector<int> buildSuffixArray(const string& s) {
    int n = s.size();

    vector<int> sa(n);
    vector<int> rank(n);
    vector<int> temp(n);

    for (int i = 0; i < n; i++) {
        sa[i] = i;
        rank[i] = s[i];
    }

    for (int k = 1; k < n; k *= 2) {

        auto cmp = [&](int a, int b) {
            if (rank[a] != rank[b]) {
                return rank[a] < rank[b];
            }

            int ra = (a + k < n) ? rank[a + k] : -1;
            int rb = (b + k < n) ? rank[b + k] : -1;

            return ra < rb;
        };

        sort(sa.begin(), sa.end(), cmp);

        temp[sa[0]] = 0;

        for (int i = 1; i < n; i++) {
            temp[sa[i]] =
                temp[sa[i - 1]] + cmp(sa[i - 1], sa[i]);
        }

        rank = temp;

        if (rank[sa[n - 1]] == n - 1) {
            break;
        }
    }

    return sa;
}

string buildBWT(
    const string& text,
    const vector<int>& sa
) {
    int n = text.size();

    string bwt;
    bwt.reserve(n);

    for (int i = 0; i < n; i++) {
        int idx = sa[i];
        bwt += text[(idx - 1 + n) % n];
    }

    return bwt;
}

FMIndex buildFMIndex(string dna) {
    if (dna.empty() || dna.back() != '$') {
        dna += '$';
    }

    FMIndex fm;

    fm.text = dna;
    fm.sa = buildSuffixArray(dna);
    fm.bwt = buildBWT(dna, fm.sa);

    int n = dna.size();
    int alphabetSize = 5;

    vector<int> count(alphabetSize, 0);

    for (char c : fm.bwt) {
        int idx = charToIndex(c);

        if (idx == -1) {
            throw runtime_error("DNA에 A, C, G, T 이외의 문자가 포함되어 있습니다.");
        }

        count[idx]++;
    }

    fm.C.assign(alphabetSize, 0);

    for (int i = 1; i < alphabetSize; i++) {
        fm.C[i] = fm.C[i - 1] + count[i - 1];
    }

    fm.occ.assign(
        alphabetSize,
        vector<int>(n + 1, 0)
    );

    for (int i = 0; i < n; i++) {

        for (int c = 0; c < alphabetSize; c++) {
            fm.occ[c][i + 1] = fm.occ[c][i];
        }

        int idx = charToIndex(fm.bwt[i]);
        fm.occ[idx][i + 1]++;
    }

    return fm;
}

void searchWithMismatchDFS(
    const FMIndex& fm,
    const string& pattern,
    int pos,
    int left,
    int right,
    int mismatches,
    int maxMismatch,
    vector<int>& bestMismatchByRow
) {
    if (left >= right) return;
    if (mismatches > maxMismatch) return;

    if (pos < 0) {

        for (int row = left; row < right; row++) {

            if (bestMismatchByRow[row] == -1 ||
                mismatches < bestMismatchByRow[row]) {

                bestMismatchByRow[row] = mismatches;
            }
        }

        return;
    }

    char bases[4] = {'A', 'C', 'G', 'T'};

    for (char base : bases) {

        int c = charToIndex(base);

        int nextMismatch = mismatches;

        if (base != pattern[pos]) {
            nextMismatch++;
        }

        if (nextMismatch > maxMismatch) {
            continue;
        }

        int newLeft =
            fm.C[c] + fm.occ[c][left];

        int newRight =
            fm.C[c] + fm.occ[c][right];

        if (newLeft < newRight) {
            searchWithMismatchDFS(
                fm,
                pattern,
                pos - 1,
                newLeft,
                newRight,
                nextMismatch,
                maxMismatch,
                bestMismatchByRow
            );
        }
    }
}

vector<MatchInfo> locatePatternWithMismatch(
    const FMIndex& fm,
    const string& pattern,
    int maxMismatch
) {
    vector<MatchInfo> matches;

    if (pattern.empty()) {
        return matches;
    }

    vector<int> bestMismatchByRow(
        fm.sa.size(),
        -1
    );

    searchWithMismatchDFS(
        fm,
        pattern,
        static_cast<int>(pattern.size()) - 1,
        0,
        static_cast<int>(fm.bwt.size()),
        0,
        maxMismatch,
        bestMismatchByRow
    );

    for (size_t row = 0; row < bestMismatchByRow.size(); row++) {

        if (bestMismatchByRow[row] != -1) {
            matches.push_back({
                fm.sa[row],
                bestMismatchByRow[row]
            });
        }
    }

    sort(
        matches.begin(),
        matches.end(),
        [](const MatchInfo& a, const MatchInfo& b) {
            if (a.mismatch != b.mismatch) {
                return a.mismatch < b.mismatch;
            }

            return a.position < b.position;
        }
    );

    return matches;
}

void runBWTMappingFile(const Config& cfg) {

    int allowedMismatch = cfg.allowedMismatch;

    if (allowedMismatch < 0) {
        throw runtime_error(
            "허용 mismatch 수는 0 이상의 정수여야 합니다."
        );
    }

    string genome =
        loadGenome("0_DNA.txt");

    vector<string> shortReads =
        loadShortReads("1_ShortReads.txt");

    FMIndex fm =
        buildFMIndex(genome);

    vector<vector<MatchInfo>> results;
    results.reserve(shortReads.size());

    for (const string& read : shortReads) {
        results.push_back(
            locatePatternWithMismatch(
                fm,
                read,
                allowedMismatch
            )
        );
    }

    vector<info> summarizedResults =
        summarizeResults(results);

    saveResults(
        "3_BWT_result.txt",
        summarizedResults
    );

    cout << "3_BWT_result.txt 저장 완료\n";
}


vector<info> BWTMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
) {
    if (allowedMismatch < 0) {
        throw runtime_error(
            "허용 mismatch 수는 0 이상의 정수여야 합니다."
        );
    }

    FMIndex fm =
        buildFMIndex(genome);

    vector<vector<MatchInfo>> results;
    results.reserve(shortReads.size());

    for (const string& read : shortReads) {
        results.push_back(
            locatePatternWithMismatch(
                fm,
                read,
                allowedMismatch
            )
        );
    }

    return summarizeResults(results);
}
