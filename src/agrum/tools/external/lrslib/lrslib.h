#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* lrslib.hpp (vertex enumeration using lexicographic reverse search) */
#define TITLE "lrslib "
#define VERSION "v.6.2 2016.3.28"
#define AUTHOR "*Copyright (c) 1995,2016, David Avis   avis@cs.mcgill.ca "

/* This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */
/*Ver 6.1   major change is new lrsnash driver and library coded by Terje
 * Lensberg */
/*Ver 6.0   major change is mplrs wrapper for multithreading coded by Skip
 * Jordan  */
/*Ver 5.0   major change is plrs wrapper for multithreading coded by Gary
 * Roumanis */
/*Ver 4.2*  library version                                      */
/******************************************************************************/
/*  See http://cgm.cs.mcgill.ca/~avis/C/lrs.html for usage instructions */
/******************************************************************************/


#ifdef PLRS
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#endif


#ifdef LRSLONG
#define ARITH "lrslong.h" /* lrs long integer arithmetic package */
#else
#ifdef GMP
#define ARITH "lrsgmp.h" /* lrs wrapper for gmp multiple precsion arithmetic */
#else
#define ARITH "lrsmp.h" /* lrs multiple precsion arithmetic    */
#define MP
#endif
#endif

#include ARITH

// 64 bits for windows (long is 32 bits)
#ifdef _MSC_VER
typedef  __int64          int64_t;
typedef unsigned  __int64 uint64_t;
#else
#include <stdint.h>
#endif

#ifdef SIGNALS
#include <signal.h>
#include <unistd.h>
#define errcheck(s, e)       \
  if ((int64_t)(e) == -1L) { \
    perror(s);               \
    exit(1);                 \
  }
#endif

#define CALLOC(n, s) xcalloc(n, s,   __LINE__,   __FILE__)

/*********************/
/*global constants   */
/*********************/
#define MAX_LRS_GLOBALS 10000L /* number of allocated dictionaries */
#define MAXIMIZE 1L            /* maximize the lp  */
#define MINIMIZE 0L            /* maximize the lp  */
#define GE 1L                  /* constraint is >= */
#define EQ 0L                  /* constraint is linearity */


/*************/
/* typedefs  */
/*************/

/******************************************************************************/
/*                   Indexing after initialization                            */
/*               Basis                                    Cobasis             */
/*   ---------------------------------------    ----------------------------- */
/*  |  i  |0|1| .... |lastdv|lastdv+1|...|m|   | j  | 0 | 1 | ... |d-1|  d  | */
/*  |-----|+|+|++++++|++++++|--------|---|-|   |----|---|---|-----|---|+++++| */
/*  |B[i] |0|1| .... |lastdv|lastdv+1|...|m|   |C[j]|m+1|m+2| ... |m+d|m+d+1| */
/*   -----|+|+|++++++|++++++|????????|???|?|    ----|???|???|-----|???|+++++| */
/*                                                                            */
/* Row[i] is row location for B[i]         Col[j] is column location for C[j] */
/*  -----------------------------              -----------------------------  */
/* |   i   |0|1| ..........|m-1|m|            | j    | 0 | 1 | ... |d-1| d  | */
/* |-------|+|-|-----------|---|-|            |------|---|---|---  |---|++++| */
/* |Row[i] |0|1|...........|m-1|m|            |Col[j]| 1 | 2 | ... | d |  0 | */
/* --------|+|*|***********|***|*|             ------|***|***|*****|***|++++| */
/*                                                                            */
/*  + = remains invariant   * = indices may be permuted ? = swapped by pivot  */
/*                                                                            */
/*  m = number of input rows   n= number of input columns                     */
/*  input dimension inputd = n-1 (H-rep) or n (V-rep)                         */
/*  lastdv = inputd-nredundcol  (each redundant column removes a dec. var)    */
/*  working dimension d=lastdv-nlinearity (an input linearity removes a slack)
 */
/*  obj function in row 0, index 0=B[0]  col 0 has index m+d+1=C[d]           */
/*  H-rep: b-vector in col 0, A matrix in columns 1..n-1                      */
/*  V-rep: col 0 all zero, b-vector in col 1, A matrix in columns 1..n        */
/******************************************************************************/

typedef struct lrs_dic_struct /* dynamic dictionary data */
{
  lrs_mp_matrix A;
  int64_t       m;       /* A has m+1 rows, row 0 is cost row            */
  int64_t       m_A;     /* =m or m-d if nonnegative flag set            */
  int64_t       d;       /* A has d+1 columns, col 0 is b-vector         */
  int64_t       d_orig;  /* value of d as A was allocated  (E.G.)        */
  int64_t       lexflag; /* true if lexmin basis for this vertex         */
  int64_t       depth;   /* depth of basis/vertex in reverse search tree */
  int64_t       i, j;    /* last pivot row and column pivot indices      */
  lrs_mp        det;     /* current determinant of basis                 */
  lrs_mp        objnum;  /* objective numerator value                    */
  lrs_mp        objden;  /* objective denominator value                  */
  int64_t *     B, *Row; /* basis, row location indices                  */
  int64_t *     C, *Col; /* cobasis, column location indices             */
  struct lrs_dic_struct *prev, *next;
} lrs_dic;

typedef struct lrs_dat /* global problem data   */
{
  lrs_mp_vector Gcd; /* Gcd of each row of numerators               */
  lrs_mp_vector Lcm; /* Lcm for each row of input denominators      */

  lrs_mp  sumdet;     /* sum of determinants                          */
  lrs_mp  Nvolume;    /* volume numerator                             */
  lrs_mp  Dvolume;    /* volume denominator                           */
  lrs_mp  boundn;     /* objective bound numerator                    */
  lrs_mp  boundd;     /* objective bound denominator                  */
  int64_t unbounded;  /* lp unbounded */
  char    fname[100]; /* input file name from line 1 of input         */

  int64_t* inequality; /* indices of inequalities corr. to cobasic ind */
  /* initially holds order used to find starting  */
  /* basis, default: m,m-1,...,2,1                */
  int64_t* facet;         /* cobasic indices for restart in needed        */
  int64_t* redundcol;     /* holds columns which are redundant            */
  int64_t* linearity;     /* holds cobasic indices of input linearities   */
  int64_t* minratio;      /* used for lexicographic ratio test            */
  int64_t* temparray;     /* for sorting indices, dimensioned to d        */
  int64_t *isave, *jsave; /* arrays for estimator, malloc'ed at start     */
  int64_t  inputd;        /* input dimension: n-1 for H-rep, n for V-rep  */

  int64_t m;      /* number of rows in input file                 */
  int64_t n;      /* number of columns in input file              */
  int64_t lastdv; /* index of last dec. variable after preproc    */
  /* given by inputd-nredundcol                   */
  int64_t count[10]; /* count[0]=rays [1]=verts. [2]=base [3]=pivots */
                     /* count[4]=integer vertices                    */

  int64_t startcount[5];

  int64_t deepest;    /* max depth ever reached in search             */
  int64_t nredundcol; /* number of redundant columns                  */
  int64_t nlinearity; /* number of input linearities                  */
  int64_t totalnodes; /* count total number of tree nodes evaluated   */
  int64_t runs;       /* probes for estimate function                 */
  int64_t seed;       /* seed for random number generator             */
  double  cest[10];   /* ests: 0=rays,1=vert,2=bases,3=vol,4=int vert */
  /**** flags  **********                         */
  int64_t allbases;  /* TRUE if all bases should be printed          */
  int64_t bound;     /* TRUE if upper/lower bound on objective given */
  int64_t countonly; /* TRUE if only count totals should be output   */
  int64_t debug;
  int64_t dualdeg;     /* TRUE if start dictionary is dual degenerate  */
  int64_t etrace;      /* turn off debug at basis # strace             */
  int64_t frequency;   /* frequency to print cobasis indices           */
  int64_t geometric;   /* TRUE if incident vertex prints after each ray */
  int64_t getvolume;   /* do volume calculation                        */
  int64_t givenstart;  /* TRUE if a starting cobasis is given          */
  int64_t homogeneous; /* TRUE if all entries in column one are zero   */
  int64_t hull;        /* do convex hull computation if TRUE           */
  int64_t incidence;   /* print all tight inequalities (vertices/rays) */
  int64_t lponly;      /* true if only lp solution wanted              */
  int64_t maxdepth;    /* max depth to search to in treee              */
  int64_t maximize;    /* flag for LP maximization                     */
  int64_t maxoutput;   /* if positive, maximum number of output lines  */
  int64_t maxcobases; /* if positive, after maxcobasis unexplored subtrees reported
                         */
  int64_t minimize;   /* flag for LP minimization                     */
  int64_t mindepth;   /* do not backtrack above mindepth              */
  int64_t nash;       /* TRUE for computing nash equilibria           */
  int64_t nonnegative;  /* TRUE if last d constraints are nonnegativity */
  int64_t polytope;     /* TRUE for facet computation of a polytope     */
  int64_t printcobasis; /* TRUE if all cobasis should be printed        */
  int64_t printslack;   /* TRUE if indices of slack inequal. printed    */
  int64_t truncate;     /* TRUE: truncate tree when moving from opt vert*/
  int64_t verbose;      /* FALSE for minimalist output                  */
  int64_t restart;      /* TRUE if restarting from some cobasis         */
  int64_t strace;       /* turn on  debug at basis # strace             */
  int64_t voronoi;      /* compute voronoi vertices by transformation   */
  int64_t subtreesize;  /* in estimate mode, iterates if cob_est >= subtreesize */

  /* Variables for saving/restoring cobasis,  db */

  int64_t id;   /* numbered sequentially */
  char*   name; /* passed by user */

  int64_t  saved_count[3]; /* How often to print out current cobasis */
  int64_t* saved_C;
  lrs_mp   saved_det;
  int64_t  saved_depth;
  int64_t  saved_d;

  int64_t saved_flag; /* There is something in the saved cobasis */

  /* Variables for cacheing dictionaries, db */
  lrs_dic *Qhead, *Qtail;

} lrs_dat, lrs_dat_p;


#ifdef PLRS
/****************/
/* 	PLRS 	*/
/****************/

void post_output(const char*, const char*);
void plrs_read_dat(lrs_dat* Q, std::ifstream& ff);
void plrs_read_dic(lrs_dic* P, lrs_dat* Q, std::ifstream& ff);
void plrs_readfacets(lrs_dat* Q, int64_t facet[], string facets);
void plrs_readlinearity(lrs_dat* Q, string line);
#endif


/*******************************/
/* functions  for external use */
/*******************************/
extern FILE* lrs_cfp; /* output file for checkpoint information       */
int64_t
lrs_main(int   argc,
         char* argv[]); /* lrs driver, argv[1]=input file, [argc-1]=output file */
int64_t
redund_main(int   argc,
            char* argv[]); /* redund driver, argv[1]=input file, [2]=output file */
lrs_dat*
lrs_alloc_dat(const char* name);    /* allocate for lrs_dat structure "name" */
lrs_dic* lrs_alloc_dic(lrs_dat* Q); /* allocate for lrs_dic structure corr. to Q */
int64_t lrs_estimate(lrs_dic* P,
                     lrs_dat* Q); /* get estimates only and returns est number of
                                     cobases in subtree */
int64_t lrs_read_dat(lrs_dat* Q,
                     int      argc,
                     char*    argv[]); /* read header and set up lrs_dat */
int64_t lrs_read_dic(lrs_dic* P,
                     lrs_dat* Q); /* read input and set up problem and lrs_dic */
int64_t lrs_checkbound(
  lrs_dic* P,
  lrs_dat* Q); /* TRUE if current objective value exceeds specified bound */
int64_t lrs_getfirstbasis(
  lrs_dic**      P_p,
  lrs_dat*       Q,
  lrs_mp_matrix* Lin,
  int64_t no_output); /* gets first basis, FALSE if none,P may get changed if
                  lin. space Lin found  no_output is TRUE supresses
                  output headers P may get changed if lin. space Lin
                  found    */
void lrs_getinput(lrs_dic* P,
                  lrs_dat* Q,
                  int64_t* num,
                  int64_t* den,
                  int64_t  m,
                  int64_t  d); /* reads input matrix b A in lrs/cdd format */
int64_t lrs_getnextbasis(lrs_dic** dict_p,
                         lrs_dat*  Q,
                         int64_t   prune); /* gets next lrs tree basis,
                                       FALSE if none backtrack if
                                       prune is TRUE */
int64_t lrs_getsolution(lrs_dic* P, lrs_dat* Q, lrs_mp_vector output, int64_t col);
int64_t lrs_getray(
  lrs_dic* P, lrs_dat* Q, int64_t col, int64_t comment, lrs_mp_vector output);
int64_t lrs_getvertex(lrs_dic* P, lrs_dat* Q, lrs_mp_vector output);
void lrs_close(char* name); /* close lrs lib program "name"                 */
int64_t lrs_init(
  char* name); /* initialize lrslib and arithmetic package for prog "name" */
void lrs_lpoutput(lrs_dic*      P,
                  lrs_dat*      Q,
                  lrs_mp_vector output); /* print LP primal and dual solutions */
void lrs_printcobasis(
  lrs_dic* P,
  lrs_dat* Q,
  int64_t  col); /* print cobasis for column col(verted or ray)  */
void lrs_printoutput(lrs_dat* Q, lrs_mp_vector output); /* print output array */
void lrs_printrow(char          name[],
                  lrs_dat*      Q,
                  lrs_mp_vector output,
                  int64_t rowd); /*print row of A matrix in output[0..rowd] */
void lrs_printsol(lrs_dic* P,
                  lrs_dat* Q,
                  int64_t  col,
                  int64_t  comment); /* print out solution from col, comment=
                     0=normal,-1=geometric
                     ray,1..inputd=linearity */
void lrs_printtotals(lrs_dic* P, lrs_dat* Q); /* print final totals for lrs */
int64_t lrs_set_digits(
  int64_t dec_digits); /* set lrsmp digits to equiv. of decimal dec_digits */
int64_t
lrs_solvelp(lrs_dic* P,
            lrs_dat* Q,
            int64_t  maximize); /* solve primal feas LP:TRUE bounded else FALSE */


/*******************************/
/* functions  for internal use */
/*******************************/


/*******************************/
/* basic dictionary functions  */
/*******************************/
int64_t getabasis(lrs_dic* P,
                  lrs_dat* Q,
                  int64_t  order[]); /* Try to find a starting basis  */
void getnextoutput(lrs_dic* P,
                   lrs_dat* Q,
                   int64_t  i,
                   int64_t  col,
                   lrs_mp   out); /* get A[B[i][col] and copy to out */
int64_t ismin(lrs_dic* P,
              lrs_dat* Q,
              int64_t  r,
              int64_t  s); /* test if A[r][s] is a min ratio for col s */
int64_t lexmin(lrs_dic* P,
               lrs_dat* Q,
               int64_t  col); /* test A to see if current basis is lexmin */
void pivot(lrs_dic* P,
           lrs_dat* Q,
           int64_t  bas,
           int64_t  cob); /* Qpivot routine for array A  */
int64_t primalfeasible(lrs_dic* P,
                       lrs_dat* Q); /* Do dual pivots to get primal feasibility */
int64_t lrs_ratio(lrs_dic* P, lrs_dat* Q, int64_t col); /* find lex min. ratio */
int64_t removecobasicindex(lrs_dic* P,
                           lrs_dat* Q,
                           int64_t  k); /* remove C[k] from problem  */
int64_t restartpivots(lrs_dic* P,
                      lrs_dat* Q); /* restart problem from given cobasis   */
int64_t reverse(lrs_dic* P,
                lrs_dat* Q,
                int64_t* r,
                int64_t  s); /* TRUE if B[*r] C[s] is a reverse lex-pos pivot  */
int64_t
selectpivot(lrs_dic* P,
            lrs_dat* Q,
            int64_t* r,
            int64_t* s); /* select pivot indices using lexicographic rule */
int64_t
dan_selectpivot(lrs_dic* P,
                lrs_dat* Q,
                int64_t* r,
                int64_t* s); /* select pivot indices using dantzig-lex rule */
void update(lrs_dic* P,
            lrs_dat* Q,
            int64_t* i,
            int64_t* j); /* update the B,C, LOC arrays after a pivot       */
void updatevolume(lrs_dic* P,
                  lrs_dat* Q); /* rescale determinant and update the volume */


/*******************************/
/* other functions using P,Q   */
/*******************************/
int64_t
lrs_degenerate(lrs_dic* P,
               lrs_dat* Q); /* TRUE if the dictionary is primal degenerate */
void print_basis(FILE* fp, lrs_dat* Q);
void printA(lrs_dic* P,
            lrs_dat* Q); /* raw print of dictionary, bases for debugging   */
void pimat(lrs_dic* P,
           int64_t  r,
           int64_t  s,
           lrs_mp   Nt,
           char     name[]); /* print the row r col s of A                     */
int64_t readfacets(lrs_dat* Q, int64_t facet[]); /* read and check facet list */
int64_t readlinearity(lrs_dat* Q); /* read and check linearity list */
void rescaledet(lrs_dic* P,
                lrs_dat* Q,
                lrs_mp   Vnum,
                lrs_mp   Vden); /* rescale determinant to get its volume */
void rescalevolume(lrs_dic* P,
                   lrs_dat* Q,
                   lrs_mp   Vnum,
                   lrs_mp   Vden); /* adjust volume for dimension          */
int64_t lrs_leaf(lrs_dic* P, lrs_dat* Q); /* true if current dictionary is leaf
                                          of reverse search tree  */


/***************************************************/
/* Routines for redundancy checking                */
/***************************************************/
int64_t checkredund(lrs_dic* P, lrs_dat* Q); /* solve primal lp to check redund
                                             of obj fun. returns TRUE if
                                             redundant, else FALSE          */
int64_t checkcobasic(lrs_dic* P,
                     lrs_dat* Q,
                     int64_t  index); /* TRUE if index is cobasic and
                                  nondegenerate  FALSE if basic, or
                                  degen. cobasic, where it will get
                                  pivoted out  */
int64_t checkindex(
  lrs_dic* P,
  lrs_dat* Q,
  int64_t  index); /* index=0 non-red.,1 red., 2 input linearity NOTE: row is
               returned all zero if redundant!!  */


/***************************************************/
/* Routines for caching and restoring dictionaries */
/***************************************************/
void lrs_free_dic(lrs_dic* P, lrs_dat* Q);
void lrs_free_dic2(lrs_dic* P, lrs_dat* Q); /* same as lrs_free_dic but no cache*/
void lrs_free_dat(lrs_dat* Q);
void copy_dict(lrs_dat* global, lrs_dic* dest, lrs_dic* src);
lrs_dic* alloc_memory(lrs_dat* Q);
lrs_dic* lrs_getdic(lrs_dat* Q);
lrs_dic* resize(lrs_dic* P, lrs_dat* Q);

/*******************************/
/* utilities                   */
/*******************************/
void lprat(const char* name,
           int64_t     Num,
           int64_t     Den); /* Print Num/Den without reducing  */
int64_t
lreadrat(int64_t* Num,
         int64_t* Den); /* read a rational string and convert to long integers */
void reorder(int64_t a[],
             int64_t range); /* reorder array in increasing order with one
                 misplaced element   */
void reorder1(int64_t a[],
              int64_t b[],
              int64_t newone,
              int64_t range); /* reorder array a in increasing order with
                                 misplaced element newone elements of b go along
                                 for the ride */

/***************************/
/* lp_solve like functions */
/***************************/
int64_t lrs_solve_lp(lrs_dic* P,
                     lrs_dat* Q); /* solve lp only for given dictionary */
void lrs_set_row(
  lrs_dic* P,
  lrs_dat* Q,
  int64_t  row,
  int64_t  num[],
  int64_t  den[],
  int64_t  ineq); /* load row i of dictionary from num[]/den[] ineq=GE */
void lrs_set_row_mp(
  lrs_dic*      P,
  lrs_dat*      Q,
  int64_t       row,
  lrs_mp_vector num,
  lrs_mp_vector den,
  int64_t       ineq); /* same as lrs_set_row except num/den is lrs_mp type */
void lrs_set_obj(lrs_dic* P,
                 lrs_dat* Q,
                 int64_t  num[],
                 int64_t  den[],
                 int64_t  max); /* set up objective function with coeffs
                num[]/den[] max=MAXIMIZE or MINIMIZE  */
void lrs_set_obj_mp(
  lrs_dic*      P,
  lrs_dat*      Q,
  lrs_mp_vector num,
  lrs_mp_vector den,
  int64_t       max); /* same as lrs_set_obj but num/den has lrs_mp type */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
