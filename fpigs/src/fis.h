#ifndef __FIS_H__
#define __FIS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif
/***********************************************************************
 Macros and definitions
 **********************************************************************/

/* Define portable printf and double */
#if defined(MATLAB_MEX_FILE)
# define PRINTF mexPrintf
# define DOUBLE real_T
#elif defined(__SIMSTRUC__)
# define PRINTF ssPrintf
# define DOUBLE real_T
#else
# define PRINTF printf
# define DOUBLE double
#endif

#ifndef ABS
# define ABS(x)   ( (x) > (0) ? (x): (-(x)) )
#endif
#ifndef MAX
# define MAX(x,y) ( (x) > (y) ? (x) : (y) )
#endif
#ifndef MIN
# define MIN(x,y) ( (x) < (y) ? (x) : (y) )
#endif
#define MF_PARA_N 4
#define STR_LEN 500
#define MF_POINT_N 101

/*
#define PRINT(expr) printf(#expr " = %g\n", (double)expr)
#define PRINTMAT(mat,m,n) printf(#mat " = \n"); fisPrintMatrix(mat,m,n)
#define FREEMAT(mat,m) printf("Free " #mat " ...\n"); fisFreeMatrix(mat,m)
#define FREEARRAY(array) printf("Free " #array " ...\n"); free(array)
*/

#if (defined(MATLAB_MEX_FILE) && !defined(__SIMSTRUC__))
# define FREE mxFree
#else
# define FREE free
#endif

#define FREEMAT(mat,m) fisFreeMatrix(mat,m)
#define FREEARRAY(array) FREE(array)

/***********************************************************************
 Data types
 **********************************************************************/

typedef struct fis_node {
	int handle;
	int load_param;
	char name[STR_LEN];
	char type[STR_LEN];
	char andMethod[STR_LEN];
	char orMethod[STR_LEN];
	char impMethod[STR_LEN];
	char aggMethod[STR_LEN];
	char defuzzMethod[STR_LEN];
	int userDefinedAnd;
	int userDefinedOr;
	int userDefinedImp;
	int userDefinedAgg;
	int userDefinedDefuzz;
	int in_n;
	int out_n;
	int rule_n;
	int **rule_list;
	DOUBLE *rule_weight;
	int *and_or;	/* AND-OR indicator */
	DOUBLE *firing_strength;
	DOUBLE *rule_output;
	/* Sugeno: output for each rules */
	/* Mamdani: constrained output MF values of rules */
	struct io_node **input;
	struct io_node **output;
	DOUBLE (*andFcn)(DOUBLE, DOUBLE);
	DOUBLE (*orFcn)(DOUBLE, DOUBLE);
	DOUBLE (*impFcn)(DOUBLE, DOUBLE);
	DOUBLE (*aggFcn)(DOUBLE, DOUBLE);
	DOUBLE (*defuzzFcn)(fis_node*, int, DOUBLE*, int);
	DOUBLE *BigOutMfMatrix;	/* used for Mamdani system only */
        DOUBLE *BigWeightMatrix;/* used for Mamdani system only */
	DOUBLE *mfs_of_rule;	/* MF values in a rule */

	DOUBLE *bias; /*bias, to be tuned when no rules are fired*/
	int isbias;

	struct fis_node *next;
} FIS;



typedef struct io_node {
	char name[STR_LEN];
	int mf_n;
	DOUBLE bound[2];
	DOUBLE value;
	struct mf_node **mf;
} IO;



typedef struct mf_node {
	char label[STR_LEN];	/* MF name */
	char type[STR_LEN];		/* MF type */
	int nparams;			/* length of params field */
	DOUBLE *params;			/* MF parameters */
	int userDefined;		/* 1 if the MF is user-defined */
	DOUBLE (*mfFcn)(DOUBLE, DOUBLE *); /* pointer to a mem. fcn */ 
	DOUBLE value;		    /* for Sugeno only */
	DOUBLE *value_array;	/* for Mamdani only, array of MF values */
} MF;

void fisError(char*);
FILE* fisOpenFile(const char*, const char*);
void* fisCalloc(int, int);
char** fisCreateMatrix(int, int, int);
void fisFreeMatrix(void**, int);
void fisPrintArray(DOUBLE*, int);
void fisPaus();

/***********************************************************************
 Parameterized membership functions
 **********************************************************************/
DOUBLE fisTrangleMf(DOUBLE, DOUBLE*);
DOUBLE fisTrapezoidMf(DOUBLE, DOUBLE*);
DOUBLE fisGaussianMF(DOUBLE, DOUBLE*);
DOUBLE fisGaussian2Mf(DOUBLE, DOUBLE*);
DOUBLE fisSigmoidMf(DOUBLE, DOUBLE*);
DOUBLE fisProductSigmoidMf(DOUBLE, DOUBLE*);
DOUBLE fisDifferenceSigmoidMf(DOUBLE, DOUBLE*);
DOUBLE fisGeneralizedBellMf(DOUBLE, DOUBLE*);
DOUBLE fisSMf(DOUBLE, DOUBLE*);
DOUBLE fisZMf(DOUBLE, DOUBLE*);
DOUBLE fisPiMF(DOUBLE, DOUBLE*);
DOUBLE fisAllMf(DOUBLE, DOUBLE*);
int fisGetMfParaN(char*);

/***********************************************************************
 T-norm and T-conorm operators
 **********************************************************************/
DOUBLE fisMin(DOUBLE, DOUBLE);
DOUBLE fisMax(DOUBLE, DOUBLE);
DOUBLE fisProduct(DOUBLE, DOUBLE);
DOUBLE fisProbOr(DOUBLE, DOUBLE);
DOUBLE fisSUM(DOUBLE, DOUBLE);
DOUBLE fisArryaOperations(DOUBLE*, int, DOUBLE(*fcn)(DOUBLE,DOUBLE));

/***********************************************************************
 Defuzzification methods
 **********************************************************************/
DOUBLE defuzzCentroud(FIS*, int, DOUBLE*, int);
DOUBLE defuzzBisector(FIS*, int, DOUBLE*, int);
DOUBLE defuzzMeanOfMax(FIS*, int, DOUBLE*, int);
DOUBLE defuzzSmallestOfMax(FIS*, int, DOUBLE*, int);
DOUBLE defuzzLargestestOfMax(FIS*, int, DOUBLE*, int);

/***********************************************************************
 Data structure: construction, printing, and destruction
 **********************************************************************/
IO* fisBuildToList(int, int*);
void fisAssignMfPointer(FIS*);
void fisAssignFunctionPointer(FIS*);
void fisPrintData(FIS*);
void fisFreeMfList(MF*, int);
void fisFreeIoList(IO*, int);
void fisFreeFisNode(FIS*);

void fisComputeOutputMfValueArray(FIS*,int);
void fisGetString2(char*,DOUBLE*,int);
void fisCheckDataStructure(FIS*);
void fisBuildFisNode(FIS*, DOUBLE**, int, int);
void fisLoadParameter(FIS*, DOUBLE**, int);
void fisLoadParamter1(FIS*, DOUBLE*, int);
FIS* fisMatchHandle(FIS*, int);
FIS* fisMatchName(FIS*, char*);
int fisFindMaxHandle(FIS*);

/***********************************************************************
 Main functions for fuzzy inference
 **********************************************************************/
void fisComputeInputMfValue(FIS*);
void fisComputeTskRuleOutput(FIS*);
void fisComputeFiringStrength(FIS*);
DOUBLE fisFinalOutputMf(FIS*, int, int);
void fisFinalOutputMf2(FIS*, int, DOUBLE*, int);

/***********************************************************************
 Evaluate the constructed FIS based on given input vector
 **********************************************************************/
void fisEvaluate(FIS*, int);
void getFisOutput(DOUBLE*, FIS*, DOUBLE*);


int compareString(char*, char*);
char* getNextLine(char*, FILE*);
DOUBLE getNumber(char*, FILE*);
void getString(char*, FILE*, DOUBLE*);
int getArray(char*, DOUBLE*);
void getMfN(char*, int, DOUBLE*, int, DOUBLE*);
DOUBLE** returnEmptyFismatrix(char*, int*, int*);
DOUBLE** returnFismatrix(char*, int*, int*);
DOUBLE** returnDataMatrix(char*, int*, int*);

#ifdef __cplusplus
}
#endif

#endif /* __FIS_H__ */
