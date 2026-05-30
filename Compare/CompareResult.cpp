#include "CompareResult.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

void compareReconstructionWithOriginal(
    const string& algorithmName,
    const string& original,
    const string& reconstructed
) {
    int originalLength =
        static_cast<int>(original.size());

    int reconstructedLength =
        static_cast<int>(reconstructed.size());

    int compareLength =
        min(originalLength, reconstructedLength);

    int matchCount = 0;
    int mismatchCount = 0;
    int unknownCount = 0;

    for (int i = 0; i < compareLength; i++) {

        if (reconstructed[i] == 'N') {
            unknownCount++;
        }
        else if (original[i] == reconstructed[i]) {
            matchCount++;
        }
        else {
            mismatchCount++;
        }
    }

    int lengthDifference =
        abs(originalLength - reconstructedLength);

    double accuracy = 0.0;

    if (originalLength > 0) {
        accuracy =
            static_cast<double>(matchCount)
            / originalLength
            * 100.0;
    }

    string outputPath =
        "8_Compare_" + algorithmName + ".txt";

    ofstream fout(outputPath);

    if (!fout) {
        throw runtime_error(
            outputPath + " 파일을 생성할 수 없습니다."
        );
    }

    fout << "====================================\n";
    fout << "Compare Result - " << algorithmName << '\n';
    fout << "====================================\n\n";

    fout << "Original Length      : "
         << originalLength << '\n';

    fout << "Reconstructed Length : "
         << reconstructedLength << '\n';

    fout << "Compared Length      : "
         << compareLength << '\n';

    fout << '\n';

    fout << "Match Count          : "
         << matchCount << '\n';

    fout << "Mismatch Count       : "
         << mismatchCount << '\n';

    fout << "Unknown Count (N)    : "
         << unknownCount << '\n';

    fout << "Length Difference    : "
         << lengthDifference << '\n';

    fout << fixed << setprecision(2);

    fout << "Accuracy             : "
         << accuracy << "%\n";

    fout.close();

    cout << outputPath << " 저장 완료\n";
}