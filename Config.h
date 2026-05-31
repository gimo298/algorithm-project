#pragma once

struct Config {
    int length = 100000;
    int LenOfReads = 15;
    int CntOfReads = 10000;
    double ErrorRate = 0.01;
    int allowedMismatch = 3;
};
