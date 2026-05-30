#include "RabinKarp.h"
#include "../Utils/IO.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

const long long BASE = 4;
const long long MOD = 1000000007;

int baseToInt(char c) {
    switch (c) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default:  return 0;
    }
}

int countRabinKarpMismatchUpTo(
    const string& genome,
    int start,
    const string& read,
    int allowedMismatch
) {
    int cnt = 0;

    for (int i = 0; i < (int)read.size(); i++) {
        if (genome[start + i] != read[i]) {
            cnt++;

            if (cnt > allowedMismatch) {
                return cnt;
            }
        }
    }

    return cnt;
}

vector<info> RabinKarpMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
) {
    vector<info> results(shortReads.size());

    for (size_t i = 0; i < shortReads.size(); i++) {

        const string& read = shortReads[i];

        info result;
        result.position = -1;
        result.mismatch = -1;
        result.ambiguous = false;

        if (read.size() > genome.size()) {
            results[i] = result;
            continue;
        }

        int n = genome.size();
        int m = read.size();

        int bestMismatch = allowedMismatch + 1;
        int bestCount = 0;

        // mismatch를 허용하는 경우:
        // 해시값이 다르더라도 후보가 될 수 있으므로 모든 위치 검사
        if (allowedMismatch > 0) {

            for (int j = 0; j <= n - m; j++) {

                int cntOfMismatch =
                    countRabinKarpMismatchUpTo(
                        genome,
                        j,
                        read,
                        allowedMismatch
                    );

                if (cntOfMismatch <= allowedMismatch) {

                    if (cntOfMismatch < bestMismatch) {

                        bestMismatch = cntOfMismatch;
                        bestCount = 1;

                        result.position = j;
                        result.mismatch = cntOfMismatch;
                        result.ambiguous = false;
                    }
                    else if (cntOfMismatch == bestMismatch) {

                        bestCount++;
                        result.ambiguous = true;
                    }
                }
            }

            results[i] = result;
            continue;
        }

        // allowedMismatch == 0인 경우:
        // Rabin-Karp exact matching 사용
        long long highPow = 1;

        for (int p = 0; p < m - 1; p++) {
            highPow = (highPow * BASE) % MOD;
        }

        long long readHash = 0;

        for (int p = 0; p < m; p++) {
            readHash =
                (readHash * BASE + baseToInt(read[p])) % MOD;
        }

        long long winHash = 0;

        for (int p = 0; p < m; p++) {
            winHash =
                (winHash * BASE + baseToInt(genome[p])) % MOD;
        }

        for (int j = 0; j <= n - m; j++) {

            if (winHash == readHash) {

                int cntOfMismatch =
                    countRabinKarpMismatchUpTo(
                        genome,
                        j,
                        read,
                        allowedMismatch
                    );

                if (cntOfMismatch == 0) {

                    if (result.position != -1) {
                        result.ambiguous = true;
                    }

                    result.position = j;
                    result.mismatch = 0;
                }
            }

            if (j < n - m) {

                winHash =
                    (
                        winHash
                        - (long long)baseToInt(genome[j])
                        * highPow % MOD
                        + MOD
                    ) % MOD;

                winHash =
                    (
                        winHash * BASE
                        + baseToInt(genome[j + m])
                    ) % MOD;
            }
        }

        results[i] = result;
    }

    return results;
}

void runRabinKarpMappingFile(const Config& cfg) {

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

    vector<info> results =
        RabinKarpMapping(
            genome,
            shortReads,
            allowedMismatch
        );

    saveResults(
        "4_RabinKarpMapping_result.txt",
        results
    );

    cout << "4_RabinKarpMapping_result.txt 저장 완료\n";
}