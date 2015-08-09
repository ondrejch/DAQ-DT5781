/*
 * Stand-alone codes for fuzzy inference systems.
 * J.-S. Roger Jang, 1994.
 * Copyright 1994-2002 The MathWorks, Inc.
 * $Revision: 1.12 $  $Date: 2002/06/17 12:47:24 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fis.h"
#include "fismain.h"

/***********************************************************************
 Main routine 
 **********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
DOUBLE get_fuzzy(DOUBLE** dataMatrix, int data_col_n)
{
    // Assume we always have one row of data in this version
    // i.e., dataMatrix is a type DOUBLE[data_col_n][1]
    const int data_row_n = 1;
    const bool debug = false;

    FIS *fis;

    DOUBLE **fisMatrix, **outputMatrix;
    char *fis_file;
    int fis_row_n, fis_col_n;

    // NOTE: We need to store this with the GUI exe somehow...
    fis_file = "fismain.fis";

    /* obtain FIS matrix */
    fisMatrix = returnFismatrix(fis_file, &fis_row_n, &fis_col_n);

    /* build FIS data structure */
    fis = (FIS *)fisCalloc(1, sizeof(FIS));
    fisBuildFisNode(fis, fisMatrix, fis_col_n, MF_POINT_N);

    /* error checking */
    if (data_col_n < fis->in_n) {
        PRINTF("Given FIS is a %d-input %d-output system.\n",
                fis->in_n, fis->out_n);
        PRINTF("Given data file does not have enough input entries.\n");
        fisFreeMatrix((void **)dataMatrix, data_row_n);
        fisFreeMatrix((void **)fisMatrix, fis_row_n);
        fisFreeFisNode(fis);
        fisError("Exiting ...");
        return -1.0;
    }

    /* debugging */
    if (debug) fisPrintData(fis);

    /* create output matrix */
    outputMatrix = (DOUBLE **)fisCreateMatrix(data_row_n, fis->out_n, sizeof(DOUBLE));

    /* evaluate FIS on each input vector */
    for (int i = 0; i < data_row_n; i++)
        getFisOutput(dataMatrix[i], fis, outputMatrix[i]);

    /* print output vector */
    if(debug) {
        for (int i = 0; i < data_row_n; i++) {
            for (int j = 0; j < fis->out_n; j++)
                PRINTF("%.12f ", outputMatrix[i][j]);
            PRINTF("\n");
        }
    }
    DOUBLE fuzzyVal = outputMatrix[0][0];

    /* clean up memory */
    fisFreeFisNode(fis);
    fisFreeMatrix((void **)dataMatrix, data_row_n);
    fisFreeMatrix((void **)fisMatrix, fis_row_n);
    fisFreeMatrix((void **)outputMatrix, data_row_n);
    return fuzzyVal;
}

#ifdef __cplusplus
}
#endif
