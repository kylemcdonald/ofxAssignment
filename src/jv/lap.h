/************************************************************************
 *
 *  lap.h
 version 1.0 - 21 june 1996
 author  Roy Jonker, MagicLogic Optimization Inc.
 
 
 header file for LAP
 *
 **************************************************************************/

/*************** CONSTANTS  *******************/

// changing this will change the accuracy
// if it gets closer to 100 it will start to significantly loose accuracy
#define BIG 100000

/*************** DATA TYPES *******************/

typedef int boolean;

/*************** TYPES      *******************/

typedef int row_t;
typedef int col_t;
typedef unsigned int cost_t;

/*************** FUNCTIONS  *******************/

extern cost_t lap(int dim, cost_t **assigncost,
                  row_t *rowsol, col_t *colsol,
                  cost_t *u, cost_t *v);
