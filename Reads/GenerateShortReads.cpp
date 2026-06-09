#include "GenerateShortReads.h"
#include "../Utils/IO.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

using namespace std;

char mutateBase(char current, mt19937& gen) {

    static const string DNA = "ACGT";

    uniform_int_distribution<int> dist(0, 3);

    char mutated;

    do {
        mutated = DNA[dist(gen)];
    } while (mutated == current);

    return mutated;
}

void generateShortReadsFile(const Config& cfg) {

    generateShortReadsFile(
        cfg,
        "0_DNA.txt",
        "1_ShortReads.txt"
    );
}

void generateShortReadsFile(
    const Config& cfg,
    const filesystem::path& genomePath,
    const filesystem::path& outputPath
) {

    int LenOfReads = cfg.LenOfReads;
    int CntOfReads = cfg.CntOfReads;
    double ErrorRate = cfg.ErrorRate;

    if (LenOfReads <= 0 || CntOfReads <= 0) {
        throw runtime_error("Read의 길이와 개수는 0보다 큰 정수여야 합니다.");
    }

    string genome = loadGenome(genomePath);

    if (LenOfReads > static_cast<int>(genome.size())) {
        throw runtime_error("Read의 길이는 genome 길이보다 클 수 없습니다.");
    }

    ofstream fout(outputPath);

    if (!fout) {
        throw runtime_error("1_ShortReads.txt 파일을 생성할 수 없습니다.");
    }

    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<int> startDist(
        0,
        static_cast<int>(genome.size()) - LenOfReads
    );

    uniform_real_distribution<double> probDist(0.0, 1.0);

    int totalMutationCount = 0;

    for (int i = 0; i < CntOfReads; ++i) {

        int start = startDist(gen);

        string read = genome.substr(start, LenOfReads);

        for (char& base : read) {

            double p = probDist(gen);

            if (p < ErrorRate) {
                base = mutateBase(base, gen);
                ++totalMutationCount;
            }
        }

        fout << read << '\n';
    }

    cout << outputPath.string() << " 저장 완료\n";
    cout << "reads 길이: " << LenOfReads << '\n';
    cout << "reads 개수: " << CntOfReads << '\n';
    cout << "mutation 발생 확률: " << ErrorRate << '\n';
    cout << "실제 mutation 발생 비율: "
         << static_cast<double>(totalMutationCount)
            / (CntOfReads * LenOfReads)
         << '\n';

    cout << "                       : "
         << totalMutationCount << '/'
         << CntOfReads * LenOfReads << '\n';
}