#include "fis.h"
#include "fismain.h"
#include <stdio.h>
#include <iostream>

int main(int argc, char **argv) {
    double** fuzzyTest = new double*[1];    
    double* fuzzyRow = new double[4];
     
    fuzzyRow[0] = 100.0;
    fuzzyRow[1] = 0.0;
    fuzzyRow[2] = 0.0;
    fuzzyRow[3] = 100.0;
    fuzzyTest[0] = &fuzzyRow[0];    

    double fuzzyVal = calc_fuzzy(fuzzyTest, 4);
    std::cout << "Fuzzy pos = " << fuzzyVal << std::endl;
    // Note: calc_fuzzy does pointer cleanup on fuzzyTest; no need to delete afterwards
}
