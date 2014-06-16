/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Template implementations for the VarInfo class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
// =========================================================================
#include <cmath>
// =========================================================================
#include <agrum/FMDP/learning/decision graph/varInfo.h>
// =========================================================================


// constants used by Gary Perlman for his code for computing chi2 critical values
#define GUM_Z_MAX       6.0          // maximum meaningful z value
#define GUM_CHI_EPSILON 0.000001     // accuracy of critchi approximation
#define GUM_CHI_MAX     99999.0      // maximum chi square value
#define GUM_LOG_SQRT_PI 0.5723649429247000870717135   // log (sqrt (pi))
#define GUM_I_SQRT_PI   0.5641895835477562869480795   // 1 / sqrt (pi)
#define GUM_BIGX        20.0         // max value to represent exp (x)
#define gum__ex(x) (((x) < -GUM_BIGX) ? 0.0 : exp (x))

namespace gum {

  // ##########################################################################
  // Constructor & destructor.
  // ##########################################################################

    // ==========================================================================
    // Default constructor
    // ==========================================================================
    VarInfo::VarInfo(const DiscreteVariable* attribute,
                     const DiscreteVariable* value) : __attr(attribute),
                                                      __value(value),
                                                      __degreeOfFreedom((value->domainSize()-1)*(attribute->domainSize()-1)),
                                                      __isRelevant(false),
                                                      __areRelevant(attribute->domainSize()*value->domainSize(),false){

      GUM_CONSTRUCTOR(VarInfo);

      for(Idx modality = 0; modality < __attr->domainSize(); modality++){
        __modality2Observations.insert(modality, new Set<const Observation<GUM_SCALAR>*>);
        __contingencyTable.insert( modality, new HashTable<Idx, Idx>());
      }

    }


    // ==========================================================================
    // Constructor with a list of Observation to iniialize
    // ==========================================================================
    VarInfo::VarInfo(const DiscreteVariable* attribute,
                     const DiscreteVariable* value,
                     const Set<const Observation*>* obsSet) : __attr(attribute),
                                                              __value(value),
                                                              __degreeOfFreedom( (__value->domainSize()-1)*(__attr->domainSize()-1) ),
                                                              __isRelevant(false),
                                                              __areRelevant(attribute->domainSize()*value->domainSize(),false){

      GUM_CONSTRUCTOR(VarInfo);

      for(Idx modality = 0; modality < __attr->domainSize(); modality++){
        __modality2Observations.insert(modality, new Set<const Observation*>);
        __contingencyTable.insert( modality, new HashTable<Idx, Idx>());
      }

      for(SetIteratorSafe<const Observation*> obsIter = obsSet->cbeginSafe(); obsIter != obsSet->cendSafe(); ++obsIter){

        __modality2Observations[(*obsIter)->modality(__attr)]->insert( (*obsIter) );

        __nbObservation++;
        __attrMarginalTable[(*obsIter)->modality(__attr)]++;
        __valueMarginalTable[(*obsIter)->modality(__attr)]++;
        (*__contingencyTable[(*obsIter)->modality(__attr)])[(*obsIter)->modality(__value)]++;

      }

      for( Idx attrModality = 0; attrModality < __attr->domainSize(); ++attrModality ){
        Idx rank = attrModality*__value->domainSize();
        for( Idx valModality = 0; valModality < __value->domainSize(); ++valModality ) {
          if( (*__contingencyTable[attrModality])[valModality] >= 5 )
              __areRelevant[rank+valModality] = true;
          __GStat += __GVal( attrModality, valModality );
        }
      }

      __checkRelevance();

      __pvalue = __probaChi2(__GStat, __degreeOfFreedom);
    }


    // ==========================================================================
    // Default destructor
    // ==========================================================================
    VarInfo::~VarInfo(){

      for(Idx modality = 0; modality < __attr->domainSize(); modality++){
        delete __modality2Observations[modality];
        delete __contingencyTable[modality];
      }
      GUM_DESTRUCTOR(VarInfo);
    }



  // ##########################################################################
  //
  // ##########################################################################

    // ==========================================================================
    //
    // ==========================================================================
    void VarInfo::addObservation( const Observation* newObs){

      // Addition of the observation to its matching set for fast splitting
      __modality2Observations[newObs->modality(__attr)]->insert( newObs );

      // Subtraction of the matching ceil GVal before updating
      __GStat -= __GVal(newObs->modality(__attr), newObs->modality(__value));

      // Updating
      __nbObservation++;
      __attrMarginalTable[newObs->modality(__attr)]++;
      __valueMarginalTable[newObs->modality(__attr)]++;
      (*__contingencyTable[newObs->modality(__attr)])[newObs->modality(__value)]++;

      if( !__isRelevant && (*__contingencyTable[newObs->modality(__attr)])[newObs->modality(__value)] >= 5 ){
        __areRelevant[newObs->modality(__attr)*__value->domainSize() + newObs->modality(__value)] = true;
        __checkRelevance();
      }

      __GStat += __GVal(newObs->modality(__attr), newObs->modality(__value));
      __pvalue = __probaChi2(__GStat, __degreeOfFreedom);
    }


    // ==========================================================================
    //
    // ==========================================================================
    double VarInfo::__GVal( Idx attrModa, Idx valueModa ){
      double ceil = (double)(*__contingencyTable[attrModa])[valueModa];
      if( ceil < 5 )
        return 0;
      double expected = (double)__attrMarginalTable[attrModa]*(double)__valueMarginalTable[valueModa]/(double)__nbObservation;

      return 2*ceil*log(ceil/expected);
    }


    // ==========================================================================
    //
    // ==========================================================================
    void VarInfo::__checkRelevance(){
        if( __isRelevant ) return;

        if( __nbObservation < 25 ) return;

        for(Idx i = 0; i < __areRelevant.size(); ++i )
          if( !__areRelevant[i] ) return;

        __isRelevant = true;
    }





    // ==========================================================================
    /// computes the probability of normal z value (used by the cache)
    // ==========================================================================
    double VarInfo::__probaZValue ( double z ) {
      double y, x, w;

      if ( z == 0.0 )
        x = 0.0;
      else {
        y = 0.5 * fabs( z );

        if ( y >= ( GUM_Z_MAX * 0.5 ) )
          x = 1.0;
        else if ( y < 1.0 ) {
          w = y*y;
          x = (((((((( 0.000124818987 * w
                       -0.001075204047 ) * w +0.005198775019 ) * w
                    -0.019198292004 ) * w +0.059054035642 ) * w
                  -0.151968751364 ) * w +0.319152932694 ) * w
                -0.531923007300 ) * w +0.797884560593 ) * y * 2.0;
        } else {
          y -= 2.0;
          x = ((((((((((((( -0.000045255659 * y
                            +0.000152529290 ) * y -0.000019538132 ) * y
                         -0.000676904986 ) * y +0.001390604284 ) * y
                       -0.000794620820 ) * y -0.002034254874 ) * y
                     +0.006549791214 ) * y -0.010557625006 ) * y
                   +0.011630447319 ) * y -0.009279453341 ) * y
                 +0.005353579108 ) * y -0.002141268741 ) * y
               +0.000535310849 ) * y +0.999936657524;
        }
      }

      return ( z > 0.0 ? (( x + 1.0 ) * 0.5 ) : (( 1.0 - x ) * 0.5 ) );
    }


    // ==========================================================================
    /// computes the probability of chi2 value (used by the cache)
    // ==========================================================================
    double VarInfo::__probaChi2 ( double x, unsigned long df ) {
      double a, y = 0, s;
      double e, c, z;
      int even; /* true if df is an even number */

      if (( x <= 0.0 ) || ( df < 1 ) )
        return ( 1.0 );

      a = 0.5 * x;

      even = ( 2*( df/2 ) ) == df;

      if ( df > 1 )
        y = gum__ex( -a );

      s = ( even ? y : ( 2.0 * __probaZValue( -sqrt( x ) ) ) );

      if ( df > 2 ) {
        x = 0.5 * ( df - 1.0 );
        z = ( even ? 1.0 : 0.5 );

        if ( a > GUM_BIGX ) {
          e = ( even ? 0.0 : GUM_LOG_SQRT_PI );
          c = log( a );

          while ( z <= x ) {
            e = log( z ) + e;
            s += gum__ex( c*z-a-e );
            z += 1.0;
          }

          return ( s );
        } else {
          e = ( even ? 1.0 : ( GUM_I_SQRT_PI / sqrt( a ) ) );
          c = 0.0;

          while ( z <= x ) {
            e = e * ( a / z );
            c = c + e;
            z += 1.0;
          }

          return ( c * y + s );
        }
      } else
        return ( s );
    }

} // End of namespace gum
