#include "fis.h"
#include "fismain.h"
#include <stdio.h>
#include <iostream>

int main(int argc, char **argv) {
    double** fuzzyTest =  new double*[1];    
    double* fuzzyRow = new double[4];
     
    fuzzyRow[0] = 0.0;
    fuzzyRow[1] = 0.0;
    fuzzyRow[2] = 100.0;
    fuzzyRow[3] = 100.0;
    fuzzyTest[0] = &fuzzyRow[0];    

    double* fuzzyPos = new double[1];
  
    fuzzyPos[0] = get_fuzzy(fuzzyTest, 4);
    std::cout << "Fuzzy pos = " << fuzzyPos[0] << std::endl;

}
