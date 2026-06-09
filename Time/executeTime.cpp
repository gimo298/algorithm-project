#include "executeTime.h"

#include "../Mapping/TrivialMapping.h"
#include "../Mapping/BWT.h"
#include "../Mapping/IndexMapping.h"
#include "../Mapping/RabinKarp.h"
#include "../Mapping/KMP.h"
#include "../Utils/IO.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

void measureAndSaveExecutionTimes(
    const Config& cfg,
    const string& genome,
    const vector<string>& shortReads
) {
    cout << "====================================\n";
    cout << "각 매핑 알고리즘별 연산 속도 측정 시작\n";
    cout << "게놈 길이: " << cfg.length << "\n";
    cout << "Short Reads 개수: " << cfg.CntOfReads << "\n";
    cout << "Short Reads 길이: " << cfg.LenOfReads << "\n";
    cout << "====================================\n\n";

    // 1. Trivial Mapping
    cout << "1. Trivial Mapping 실행 중...\n";
    auto start = chrono::high_resolution_clock::now();
    vector<info> trivialResults = trivialMapping(genome, shortReads, cfg.allowedMismatch);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> trivialTime = end - start;
    cout << "Trivial Mapping 완료. 소요 시간: " << trivialTime.count() << " ms\n\n";

    // 2. BWT Mapping
    cout << "2. BWT Mapping 실행 중...\n";
    start = chrono::high_resolution_clock::now();
    vector<info> bwtResults = BWTMapping(genome, shortReads, cfg.allowedMismatch);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> bwtTime = end - start;
    cout << "BWT Mapping 완료. 소요 시간: " << bwtTime.count() << " ms\n\n";

    // 3. Rabin-Karp Mapping
    cout << "3. Rabin-Karp Mapping 실행 중...\n";
    start = chrono::high_resolution_clock::now();
    vector<info> rabinKarpResults = RabinKarpMapping(genome, shortReads, cfg.allowedMismatch);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> rabinKarpTime = end - start;
    cout << "Rabin-Karp Mapping 완료. 소요 시간: " << rabinKarpTime.count() << " ms\n\n";

    // 4. Index Mapping
    cout << "4. Index Mapping 실행 중...\n";
    start = chrono::high_resolution_clock::now();
    vector<info> indexResults = indexMapping(genome, shortReads, cfg.allowedMismatch);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> indexTime = end - start;
    cout << "Index Mapping 완료. 소요 시간: " << indexTime.count() << " ms\n\n";

    // 5. KMP Mapping
    cout << "5. KMP Mapping 실행 중...\n";
    start = chrono::high_resolution_clock::now();
    vector<info> kmpResults = KMPMapping(genome, shortReads, cfg.allowedMismatch);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> kmpTime = end - start;
    cout << "KMP Mapping 완료. 소요 시간: " << kmpTime.count() << " ms\n\n";

    // TXT 파일에 결과 저장
    string outputPath = "9_ExecutionTime.txt";
    ofstream fout(outputPath);
    if (!fout) {
        cerr << outputPath << " 파일을 생성할 수 없습니다.\n";
        return;
    }

    fout << "==================================================\n";
    fout << "       Mapping Algorithms Execution Time Report   \n";
    fout << "==================================================\n";
    fout << " Genome Length      : " << cfg.length << "\n";
    fout << " Reads Count        : " << cfg.CntOfReads << "\n";
    fout << " Reads Length       : " << cfg.LenOfReads << "\n";
    fout << " Allowed Mismatches : " << cfg.allowedMismatch << "\n";
    fout << "==================================================\n";
    fout << left << setw(25) << " Algorithm" << "Execution Time (ms)\n";
    fout << "--------------------------------------------------\n";
    fout << left << setw(25) << " Trivial Mapping" << fixed << setprecision(2) << trivialTime.count() << " ms\n";
    fout << left << setw(25) << " BWT Mapping" << fixed << setprecision(2) << bwtTime.count() << " ms\n";
    fout << left << setw(25) << " Rabin-Karp Mapping" << fixed << setprecision(2) << rabinKarpTime.count() << " ms\n";
    fout << left << setw(25) << " Index Mapping" << fixed << setprecision(2) << indexTime.count() << " ms\n";
    fout << left << setw(25) << " KMP Mapping" << fixed << setprecision(2) << kmpTime.count() << " ms\n";
    fout << "==================================================\n";
    
    fout.close();
    cout << outputPath << " 저장 완료\n";
}
