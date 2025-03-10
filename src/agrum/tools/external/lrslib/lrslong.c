/* lrslong.c      library code for lrs extended precision arithmetic */
/* Version 4.0, April 13, 2000                                       */
/* Copyright: David Avis 1999, avis@cs.mcgill.ca                     */

/* Derived from prs_single.c ( rational arithmetic for lrs and prs)  */
/* authored by  Ambros Marzetta    Revision 1.2  1998/05/27          */

#ifdef PLRS
#include <sstream>
#include <iostream>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lrslong.h"

int64_t lrs_digits;		/* max permitted no. of digits   */
int64_t lrs_record_digits;		/* this is the biggest acheived so far.     */


#define MAXINPUT 1000		/*max length of any input rational */

void
gcd (lrs_mp u, lrs_mp v)
     /* Returns u=gcd(u,v) using classic Euclid's algorithm.
        v is destroyed.  Knuth, II, p.320 */

{
  uint64_t ul, vl, r;

  ul = labs (*u);
  vl = labs (*v);
  if (ul == 0)
    {
      *u = vl;
      return;
    }

  while (vl != 0)
    {
      r = ul % vl;
      ul = vl;
      vl = r;
    }
  *u = ul;
}				/* gcd */

void
lcm (lrs_mp a, lrs_mp b)			/* a = least common multiple of a, b; b is preserved */
{
  lrs_mp u, v;
  copy (u, a);
  copy (v, b);
  gcd (u, v);
  exactdivint (a, u, v);		/* v=a/u   a contains remainder = 0 */
  mulint (v, b, a);
}				/* end of lcm */


/***************************************************************/
/*                                                             */
/*     Package of routines for rational arithmetic             */
/*     (Built on top of package for multiprecision arithmetic  */
/*                                                             */
/***************************************************************/

void
reduce (lrs_mp Na, lrs_mp Da)	/* reduces Na/Da by gcd(Na,Da) */
{
  lrs_mp Nb, Db, Nc, Dc;
  copy (Nb, Na);
  copy (Db, Da);
  storesign (Nb, POS);
  storesign (Db, POS);
  copy (Nc, Na);
  copy (Dc, Da);
  gcd (Nb, Db);			/* Nb is the gcd(Na,Da) */
  exactdivint (Nc, Nb, Na);
  exactdivint (Dc, Nb, Da);
}

void
reduceint (lrs_mp Na, lrs_mp Da)	/* divide Na by Da and return */
{
  lrs_mp Temp;
  copy (Temp, Na);
  exactdivint (Temp, Da, Na);
}


int64_t
comprod (lrs_mp Na, lrs_mp Nb, lrs_mp Nc, lrs_mp Nd)
					    /* +1 if Na*Nb > Nc*Nd  */
					    /* -1 if Na*Nb < Nc*Nd  */
					    /*  0 if Na*Nb = Nc*Nd  */
{
  int64_t mc;
  mc = *Na * *Nb - *Nc * *Nd;
  if (mc > 0)
    return 1;
  if (mc < 0)
    return -1;
  return 0;
}

void
linrat (lrs_mp Na, lrs_mp Da, int64_t ka, lrs_mp Nb,  lrs_mp Db, int64_t kb, lrs_mp Nc, lrs_mp Dc)
/* computes Nc/Dc = ka*Na/Da  +kb* Nb/Db and reduces answer by gcd(Nc,Dc) */
{
  lrs_mp c;
  mulint (Na, Db, Nc);
  mulint (Da, Nb, c);
  linint (Nc, ka, c, kb);	/* Nc = (ka*Na*Db)+(kb*Da*Nb)  */
  mulint (Da, Db, Dc);		/* Dc =  Da*Db           */
  reduce (Nc, Dc);
}


void
divrat (lrs_mp Na, lrs_mp Da, lrs_mp Nb, lrs_mp Db, lrs_mp Nc, lrs_mp Dc)
/* computes Nc/Dc = (Na/Da)  / ( Nb/Db ) and reduces answer by gcd(Nc,Dc) */
{
  mulint (Na, Db, Nc);
  mulint (Da, Nb, Dc);
  reduce (Nc, Dc);
}


void
mulrat (lrs_mp Na, lrs_mp Da, lrs_mp Nb, lrs_mp Db, lrs_mp Nc, lrs_mp Dc)
/* computes Nc/Dc = Na/Da  * Nb/Db and reduces answer by gcd(Nc,Dc) */
{
  mulint (Na, Nb, Nc);
  mulint (Da, Db, Dc);
  reduce (Nc, Dc);
}

/***************************************************************/
/*                                                             */
/*     Conversion and I/O functions                            */
/*                                                             */
/***************************************************************/

void
atomp (const char *s, lrs_mp a)	/*convert string to lrs_mp integer */
     /* based on  atoi KR p.58 */
{
  int64_t diff, ten, i, sig;
  lrs_mp mpone;
  itomp (ONE, mpone);
  ten = 10L;
  for (i = 0; s[i] == ' ' || s[i] == '\n' || s[i] == '\t'; i++);
  /*skip white space */
  sig = POS;
  if (s[i] == '+' || s[i] == '-')	/* sign */
    sig = (s[i++] == '+') ? POS : NEG;
  itomp (0L, a);
  while (s[i] >= '0' && s[i] <= '9')
    {
      diff = s[i] - '0';
      linint (a, ten, mpone, diff);
      i++;
    }
  storesign (a, sig);
  if (s[i])
    {
      fprintf (stderr, "\nIllegal character in number: '%s'\n", s + i);
      exit (1);
    }
}				/* end of atomp */

void
atoaa (const char *in, char *num, char *den)
     /* convert rational string in to num/den strings */
{
  int64_t i, j;
  for (i = 0; in[i] != '\0' && in[i] != '/'; i++)
    num[i] = in[i];
  num[i] = '\0';
  den[0] = '\0';
  if (in[i] == '/')
    {
      for (j = 0; in[j + i + 1] != '\0'; j++)
	den[j] = in[i + j + 1];
      den[j] = '\0';
    }
}				/* end of atoaa */

void
mptodouble (lrs_mp a, double *x)	/* convert lrs_mp to double */
{
  (*x) = (*a);
}

int64_t
mptoi (lrs_mp a)        /* convert lrs_mp to long */
{
  return (*a);
}

void
rattodouble (lrs_mp a, lrs_mp b, double *x)	/* convert lrs_mp rati
						   onal to double */

{
  double y;
  mptodouble (a, &y);
  mptodouble (b, x);
  *x = y / (*x);
}

int64_t
readrat (lrs_mp Na, lrs_mp Da)	/* read a rational or integer and convert to lrs_mp */
	       /* returns true if denominator is not one       */
{
  char in[MAXINPUT], num[MAXINPUT], den[MAXINPUT];
  if(fscanf (lrs_ifp, "%s", in)==EOF)
                 {
                   fprintf (lrs_ofp, "\nInvalid input: check you have entered enough data!\n");
                   exit(1);
                 }

  if(!strcmp(in,"end"))          /*premature end of input file */
    {
     return (999L);
    }

  atoaa (in, num, den);		/*convert rational to num/dem strings */
  atomp (num, Na);
  if (den[0] == '\0')
    {
      itomp (1L, Da);
      return (FALSE);
    }
  atomp (den, Da);
  return (TRUE);
}

#ifdef PLRS
/* read a rational or integer and convert to lrs_mp with base BASE */
/* returns true if denominator is not one                      */
int64_t plrs_readrat (lrs_mp Na, lrs_mp Da, const char* rat)
{
  	char in[MAXINPUT], num[MAXINPUT], den[MAXINPUT];
 	strcpy(in, rat);
	atoaa (in, num, den);		/*convert rational to num/dem strings */
	atomp (num, Na);
	if (den[0] == '\0')
	{
		itomp (1L, Da);
		return (FALSE);
	}
	atomp (den, Da);
	return (TRUE);
}

#endif

void
readmp (lrs_mp a)		/* read an integer and convert to lrs_mp */
{
  int64_t in;
  if(fscanf (lrs_ifp, "%ld", &in)==EOF)
                 {
                   fprintf (lrs_ofp, "\nInvalid integer input");
                   exit(1);
                 }

  itomp (in, a);
}

#ifdef PLRS

string prat (char name[], lrs_mp Nin, lrs_mp Din)	/*reduce and print Nin/Din  */
{

	//create stream to collect output
	stringstream ss;
	string str;

	lrs_mp Nt, Dt;
	copy (Nt, Nin);
	copy (Dt, Din);
	reduce (Nt, Dt);
	if (sign (Nt) != NEG)
		ss<<" ";
	ss<<name<<*Nt;
	if (*Dt != 1)
		ss<<"/"<<*Dt;
	ss<<" ";
	//pipe stream to single string
	str = ss.str();
	return str;
}

char *cprat (char name[], lrs_mp Nin, lrs_mp Din)
{
        char *ret;
        uint64_t len;
        int i, offset=0;
	string s;
        const char *cstr;

	s = prat(name,Nin,Din);
	cstr = s.c_str();
        len = strlen(cstr);
        ret = (char *)malloc(sizeof(char)*(len+1));

        for (i=0; i+offset<len+1;)
        {
                if (cstr[i+offset]!=' ')
                {
                        ret[i] = cstr[i+offset];
                        i++;
                }
                else /* skip whitespace */
                        offset++;
        }

        return ret;
}

string pmp (char name[], lrs_mp Nt)	/*print the long precision integer a */
{

	//create stream to collect output
	stringstream ss;
	string str;

	ss<<name;
	if(sign(Nt) != NEG)
		ss<<" ";
	ss<<*Nt<<" ";

	//pipe stream to single string
	str = ss.str();
	return str;
}
#else
void
pmp (char name[], lrs_mp Nt)
{
  fprintf (lrs_ofp, "%s", name);
  if (sign (Nt) != NEG)
    fprintf (lrs_ofp, " ");
  fprintf (lrs_ofp, "%ld", *Nt);
  fprintf (lrs_ofp, " ");
}

void
prat (char name[], lrs_mp Nin, lrs_mp Din)
     /*print the long precision rational Nt/Dt  */
{
  lrs_mp Nt, Dt;
  copy (Nt, Nin);
  copy (Dt, Din);
  reduce (Nt, Dt);
  if (sign (Nt) != NEG)
    fprintf (lrs_ofp, " ");
  fprintf (lrs_ofp, "%s%ld", name, *Nt);
  if (*Dt != 1)
    fprintf (lrs_ofp, "/%ld", *Dt);
  fprintf (lrs_ofp, " ");
}				/* prat */
#endif


/***************************************************************/
/*                                                             */
/*     Memory allocation functions                             */
/*                                                             */
/***************************************************************/
lrs_mp_t
lrs_alloc_mp_t ()
 /* dynamic allocation of lrs_mp number */
{
  lrs_mp_t p;
  p=(int64_t *)calloc (1, sizeof (int64_t));
  return p;
}


lrs_mp_vector
lrs_alloc_mp_vector (int64_t n)
 /* allocate lrs_mp_vector for n+1 lrs_mp numbers */
{
  lrs_mp_vector p;
  int64_t i;

  p = (int64_t **) CALLOC ((n + 1), sizeof (lrs_mp *));
  for (i = 0; i <= n; i++)
    p[i] = (int64_t *) CALLOC (1, sizeof (lrs_mp));

  return p;
}

void
lrs_clear_mp_vector (lrs_mp_vector p, int64_t n)
/* free space allocated to p */
{
  int64_t i;
  for (i = 0; i <= n; i++)
    free (p[i]);
  free (p);
}

lrs_mp_matrix
lrs_alloc_mp_matrix (int64_t m, int64_t n)
/* allocate lrs_mp_matrix for m+1 x n+1 lrs_mp numbers */
{
  lrs_mp_matrix a;
  int64_t *araw;
  int mp_width, row_width;
  int i, j;

  mp_width = lrs_digits + 1;
  row_width = (n + 1) * mp_width;

  araw = (int64_t *) calloc ((m + 1) * row_width, sizeof (int64_t));
  a = (int64_t ***) calloc ((m + 1), sizeof (lrs_mp_vector));

  for (i = 0; i < m + 1; i++)
    {
      a[i] = (int64_t**) calloc ((n + 1), sizeof (lrs_mp *));

      for (j = 0; j < n + 1; j++)
	a[i][j] = (araw + i * row_width + j * mp_width);
    }
  return a;
}

void
lrs_clear_mp_matrix (lrs_mp_matrix p, int64_t m, int64_t n)
/* free space allocated to lrs_mp_matrix p */
{
  int64_t i;

/* p[0][0] is araw, the actual matrix storage address */

 free(p[0][0]);

 for (i = 0; i < m + 1; i++)
      free (p[i]);
/* 2015.9.9 memory leak fix */
 free(p);
}

void
lrs_getdigits (int64_t *a, int64_t *b)
{
/* send digit information to user */
  *a = DIG2DEC (ZERO);
  *b = DIG2DEC (ZERO);
  return;
}

void *
xcalloc (int64_t n, int64_t s, int64_t l, char *f)
{
  void *tmp;

  tmp = calloc (n, s);
  if (tmp == 0)
    {
      char buf[200];

      snprintf (buf, 200, "\n\nFatal error on line %ld of %s", l, f);
      perror (buf);
      exit (1);
    }
  return tmp;
}

int64_t
lrs_mp_init (int64_t dec_digits, FILE * fpin, FILE * fpout)
/* max number of decimal digits for the computation */
/* long int version                                 */
{
#ifndef PLRS
  lrs_ifp = fpin;
  lrs_ofp = fpout;
#endif
  lrs_record_digits = 0;


  lrs_digits = 0;		/* max permitted no. of digits   */

  return TRUE;
}

void
notimpl (char s[])
{
  fflush (stdout);
  fprintf (stderr, "\nAbnormal Termination  %s\n", s);
  exit (1);
}

/***************************************************************/
/*                                                             */
/*     Misc. functions                                         */
/*                                                             */
/***************************************************************/

void
reducearray (lrs_mp_vector p, int64_t n)
/* find largest gcd of p[0]..p[n-1] and divide through */
{
  lrs_mp divisor;
  lrs_mp Temp;
  int64_t i = 0L;

  while ((i < n) && zero (p[i]))
    i++;
  if (i == n)
    return;

  copy (divisor, p[i]);
  storesign (divisor, POS);
  i++;

  while (i < n)
    {
      if (!zero (p[i]))
	{
	  copy (Temp, p[i]);
	  storesign (Temp, POS);
	  gcd (divisor, Temp);
	}
      i++;
    }

/* reduce by divisor */
  for (i = 0; i < n; i++)
    if (!zero (p[i]))
      reduceint (p[i], divisor);
}				/* end of reducearray */


int64_t
myrandom (int64_t num, int64_t nrange)
/* return a random number in range 0..nrange-1 */

{
  int64_t i;
  i = (num * 401 + 673) % nrange;
  return (i);
}

void
getfactorial (lrs_mp factorial, int64_t k)		/* compute k factorial
						   in lrs_mp */
{
  lrs_mp temp;
  int64_t i;
  itomp (ONE, factorial);
  for (i = 2; i <= k; i++)
    {
      itomp (i, temp);
      mulint (temp, factorial, factorial);
    }
}				/* end of getfactorial */

void
stringcpy (char *s, char *t)	/*copy t to s pointer version */
{
  while (((*s++) = (*t++)) != '\0');
}
