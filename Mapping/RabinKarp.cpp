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

int countMismatch(
    const string& genome,
    int start,
    const string& read,
    int allowedMismatch
) {
    int cnt = 0;
    for (int i = 0; i < (int)read.size(); i++) {
        if (genome[start + i] != read[i]) {
            cnt++;
            if (cnt > allowedMismatch) return cnt;
        }
    }
    return cnt;
}

// 롤링 해시를 이용하여 블록 서열과 일치하는 genome 상의 위치 반환
vector<int> findBlockPositions(
    const string& genome,
    const string& block,
    int genomeSize,
    int blockSize
) {
    vector<int> blockPositions;

    long long highPow = 1;
    for (int p = 0; p < blockSize - 1; p++)
        highPow = (highPow * BASE) % MOD;

    long long blockHash = 0;
    for (int p = 0; p < blockSize; p++)
        blockHash = (blockHash * BASE + baseToInt(block[p])) % MOD;

    long long winHash = 0;
    for (int p = 0; p < blockSize; p++)
        winHash = (winHash * BASE + baseToInt(genome[p])) % MOD;

    for (int j = 0; j <= genomeSize - blockSize; j++) {
        if (winHash == blockHash) {
            // 해시 충돌 가능성이 있으므로 실제 문자 대조 검증
            bool match = true;
            for (int k = 0; k < blockSize; k++) {
                if (genome[j + k] != block[k]) { match = false; break; }
            }
            if (match) blockPositions.push_back(j);
        }
        // 롤링 해시 값 갱신 (O(1))
        if (j < genomeSize - blockSize) {
            winHash = (winHash - (long long)baseToInt(genome[j]) * highPow % MOD + MOD) % MOD;
            winHash = (winHash * BASE + baseToInt(genome[j + blockSize])) % MOD;
        }
    }
    return blockPositions;
}

vector<info> RabinKarpMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
) {
    vector<info> results(shortReads.size());
    int n = genome.size();

    for (size_t i = 0; i < shortReads.size(); i++) {
        const string& read = shortReads[i];
        info result;
        result.position = -1;
        result.mismatch = -1;
        result.ambiguous = false;

        if (read.size() > genome.size()) { results[i] = result; continue; }

        int m = read.size();
        int bestMismatch = allowedMismatch + 1;

        // 허용 미스매치가 0인 경우: 정확 매칭 수행
        if (allowedMismatch == 0) {
            long long highPow = 1;
            for (int p = 0; p < m - 1; p++) highPow = (highPow * BASE) % MOD;

            long long readHash = 0;
            for (int p = 0; p < m; p++) readHash = (readHash * BASE + baseToInt(read[p])) % MOD;

            long long winHash = 0;
            for (int p = 0; p < m; p++) winHash = (winHash * BASE + baseToInt(genome[p])) % MOD;

            for (int j = 0; j <= n - m; j++) {
                if (winHash == readHash) {
                    int cnt = countMismatch(genome, j, read, 0);
                    if (cnt == 0) {
                        if (result.position != -1) result.ambiguous = true;
                        result.position = j;
                        result.mismatch = 0;
                    }
                }
                if (j < n - m) {
                    winHash = (winHash - (long long)baseToInt(genome[j]) * highPow % MOD + MOD) % MOD;
                    winHash = (winHash * BASE + baseToInt(genome[j + m])) % MOD;
                }
            }
            results[i] = result;
            continue;
        }

        // 허용 미스매치가 0보다 큰 경우: 블록 분할 방식 적용
        // 리드를 (allowedMismatch + 1)개 블록으로 쪼갬 (비둘기집 원리 적용)
        int numBlocks = allowedMismatch + 1;
        int blockSize = m / numBlocks;

        // 블록이 너무 짧으면 전수조사 방식으로 fallback
        if (blockSize < 3) {
            for (int j = 0; j <= n - m; j++) {
                int cnt = countMismatch(genome, j, read, allowedMismatch);
                if (cnt <= allowedMismatch) {
                    if (cnt < bestMismatch) {
                        bestMismatch = cnt;
                        result.position = j;
                        result.mismatch = cnt;
                        result.ambiguous = false;
                    } else if (cnt == bestMismatch) {
                        result.ambiguous = true;
                    }
                }
            }
            results[i] = result;
            continue;
        }

        vector<bool> checked(n, false);  // 중복 매핑 검증 방지용

        for (int b = 0; b < numBlocks; b++) {
            int blockStart = b * blockSize;
            // 마지막 블록은 남은 길이를 모두 할당
            int curBlockSize = (b == numBlocks - 1) ? (m - blockStart) : blockSize;
            string block = read.substr(blockStart, curBlockSize);

            // 해당 블록에 대해 genome 상의 후보 매칭 위치 검색
            vector<int> blockPositions = findBlockPositions(genome, block, n, curBlockSize);

            for (int blockPos : blockPositions) {
                int readStart = blockPos - blockStart;  // 리드의 전체 매핑 시작점 계산
                if (readStart < 0 || readStart + m > n) continue;
                if (checked[readStart]) continue;
                checked[readStart] = true;

                // 후보 위치를 바탕으로 전체 리드 범위에 대해 미스매치 검사
                int cnt = countMismatch(genome, readStart, read, allowedMismatch);
                if (cnt <= allowedMismatch) {
                    if (cnt < bestMismatch) {
                        bestMismatch = cnt;
                        result.position = readStart;
                        result.mismatch = cnt;
                        result.ambiguous = false;
                    } else if (cnt == bestMismatch) {
                        result.ambiguous = true;
                    }
                }
            }
        }
        results[i] = result;
    }
    return results;
}

void runRabinKarpMappingFile(const Config& cfg) {
    if (cfg.allowedMismatch < 0)
        throw runtime_error("허용 mismatch 수는 0 이상의 정수여야 합니다.");

    string genome = loadGenome("0_DNA.txt");
    vector<string> shortReads = loadShortReads("1_ShortReads.txt");
    vector<info> results = RabinKarpMapping(genome, shortReads, cfg.allowedMismatch);
    saveResults("4_RabinKarpMapping_result.txt", results);
    cout << "4_RabinKarpMapping_result.txt 저장 완료\n";
}