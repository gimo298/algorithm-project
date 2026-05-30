#include "GenerateDNA.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string>

using namespace std;

string generateDNA(int length) {

    string bases = "ACGT";

    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<int> dist(0, 3);

    string dna;

    for (int i = 0; i < length; i++) {
        dna += bases[dist(gen)];
    }

    return dna;
}

void generateDNAFile(const Config& cfg) {

    string dna = generateDNA(cfg.length);

    ofstream fout("0_DNA.txt");

    fout << dna;

    fout.close();

    cout << "0_DNA.txt 저장 완료\n";
}