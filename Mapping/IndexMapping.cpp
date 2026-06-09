#include "IndexMapping.h"
#include "../Utils/IO.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

//  Genome의 모든 read 길이 부분 문자열을 인덱스로 저장
unordered_map<string, vector<int>> createIndex(
    const string& genome,
    int readLength
) {
    unordered_map<string, vector<int>> index;
    // 유효하지 않은 길이 예외 처리
    if (readLength <= 0 ||
        readLength > static_cast<int>(genome.size())) {
        return index;
    }
    // Genome 전체를 순회하며 인덱스 생성
    for (size_t i = 0;
         i <= genome.size() - readLength;
         i++) {

        string piece =
            genome.substr(i, readLength);
        // 같은 서열이 여러 위치에 존재할 수 있으므로 저장
        index[piece].push_back(
            static_cast<int>(i)
        );
    }

    return index;
}

// Index Mapping
vector<info> indexMapping(
    const string& genome,
    const vector<string>& shortReads,
    int allowedMismatch
) {
    vector<info> results(shortReads.size());

    if (shortReads.empty()) {
        return results;
    }
    
    // Read 길이 기준으로 인덱스 생성
    int readLength =
        static_cast<int>(shortReads[0].size());

    unordered_map<string, vector<int>> index =
        createIndex(genome, readLength);

    // Read 하나씩 매핑 수행
    for (size_t i = 0;
         i < shortReads.size();
         i++) {

        const string& shortRead =
            shortReads[i];

        info result;
        result.position = -1;
        result.mismatch = -1;
        result.ambiguous = false;

        // Read가 Genome보다 긴 경우 매핑 불가
        if (shortRead.size() > genome.size()) {
            results[i] = result;
            continue;
        }

        // 1. exact match 먼저 확인
        auto it = index.find(shortRead);

        if (it != index.end()) {

            const vector<int>& positions =
                it->second;
            // 첫 번째 위치 저장
            result.position =
                positions[0];
            
            // 동일 서열이 여러 번 발견되면 ambiguous 처리
            result.mismatch = 0;

            if (positions.size() >= 2) {
                result.ambiguous = true;
            }

            results[i] = result;
            continue;
        }

        // 2. exact match가 없을 때 mismatch 허용 탐색
        int bestMismatch =
            allowedMismatch + 1;
        
        int bestCount = 0;

        // Genome 전체 탐색
        for (size_t j = 0;
             j <= genome.size() - shortRead.size();
             j++) {

            int cntOfMismatch = 0;

            // mismatch 개수 계산
            for (size_t k = 0;
                 k < shortRead.size();
                 k++) {

                if (genome[j + k] != shortRead[k]) {
                    cntOfMismatch++;
                }
                // 허용 범위 초과 시 탐색 중단
                if (cntOfMismatch > allowedMismatch) {
                    break;
                }
            }
            // 허용 mismatch 이내인 경우
            if (cntOfMismatch <= allowedMismatch) {
                // 더 좋은 결과 발견
                if (cntOfMismatch < bestMismatch) {

                    bestMismatch = cntOfMismatch;
                    result.position =
                        static_cast<int>(j);
                    result.mismatch =
                        cntOfMismatch;
                    bestCount = 1;
                    result.ambiguous = false;
                }
                // 동일한 최적 결과가 여러 개 존재
                else if (cntOfMismatch == bestMismatch) {

                    bestCount++;
                    result.ambiguous = true;
                }
            }
        }

        results[i] = result;
    }

    return results;
}
// 파일 입출력 포함 Index Mapping 실행 함수
void runIndexMappingFile(const Config& cfg) {

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
        indexMapping(
            genome,
            shortReads,
            allowedMismatch
        );

    saveResults(
        "5_IndexMapping_result.txt",
        results
    );

    cout << "5_IndexMapping_result.txt 저장 완료\n";
}
