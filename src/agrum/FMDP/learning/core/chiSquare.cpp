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
 * @brief Sources for the ChiSquare class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <cmath>
// =========================================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
// =========================================================================


// constants used by Gary Perlman for his code for computing chi2 critical
// values

namespace gum {

  // ==========================================================================
  /// computes the probability of normal z value (used by the cache)
  // ==========================================================================
  double ChiSquare::__probaZValue( double z ) {
    //      ++nbZt;

    //      z = std::round(z * std::pow(10, 3)) / std::pow(10, 3);
    //      if( !__ZCache.exists(z) ){

    double y, x, w;

    if ( z == 0.0 )
      x = 0.0;
    else {
      y = 0.5 * fabs( z );

      if ( y >= ( __Z_MAX * 0.5 ) )
        x = 1.0;
      else if ( y < 1.0 ) {
        w = y * y;
        x = ( ( ( ( ( ( ( ( 0.000124818987 * w - 0.001075204047 ) * w +
                          0.005198775019 ) *
                            w -
                        0.019198292004 ) *
                          w +
                      0.059054035642 ) *
                        w -
                    0.151968751364 ) *
                      w +
                  0.319152932694 ) *
                    w -
                0.531923007300 ) *
                  w +
              0.797884560593 ) *
            y * 2.0;
      } else {
        y -= 2.0;
        x = ( ( ( ( ( ( ( ( ( ( ( ( ( -0.000045255659 * y + 0.000152529290 ) * y -
                                    0.000019538132 ) *
                                      y -
                                  0.000676904986 ) *
                                    y +
                                0.001390604284 ) *
                                  y -
                              0.000794620820 ) *
                                y -
                            0.002034254874 ) *
                              y +
                          0.006549791214 ) *
                            y -
                        0.010557625006 ) *
                          y +
                      0.011630447319 ) *
                        y -
                    0.009279453341 ) *
                      y +
                  0.005353579108 ) *
                    y -
                0.002141268741 ) *
                  y +
              0.000535310849 ) *
                y +
            0.999936657524;
      }
    }

    //        __ZCache.insert(z, ( z > 0.0 ? (( x + 1.0 ) * 0.5 ) : (( 1.0 - x )
    //        * 0.5 ) ) );
    //      } else {
    //          ++nbZ;
    //        }

    //      return __ZCache[z];
    return ( z > 0.0 ? ( ( x + 1.0 ) * 0.5 ) : ( ( 1.0 - x ) * 0.5 ) );
  }


  // ==========================================================================
  /// computes the probability of chi2 value (used by the cache)
  // ==========================================================================
  double ChiSquare::probaChi2( double x, Size df ) {
    double retVal = 0.0;
    //      ++nbChit;

    //      std::pair<double, unsigned long> conty(x, df);
    //      if( !__chi2Cache.exists(conty) ){

    double a, y = 0, s;
    double e, c, z;
    int    even; /* true if df is an even number */

    if ( ( x <= 0.0 ) || ( df < 1 ) ) {
      //          __chi2Cache.insert(conty,1.0);
      retVal = 1.0;
    } else {

      a = 0.5 * x;

      even = ( 2 * ( df / 2 ) ) == df;

      if ( df > 1 ) y = __exp( -a );

      s = ( even ? y : ( 2.0 * __probaZValue( -sqrt( x ) ) ) );

      if ( df > 2 ) {
        x = 0.5 * ( df - 1.0 );
        z = ( even ? 1.0 : 0.5 );

        if ( a > __BIGX ) {
          e = ( even ? 0.0 : __LOG_SQRT_PI );
          c = log( a );

          while ( z <= x ) {
            e = log( z ) + e;
            s += __exp( c * z - a - e );
            z += 1.0;
          }

          //              __chi2Cache.insert(conty,s);
          retVal = s;

        } else {
          e = ( even ? 1.0 : ( __I_SQRT_PI / sqrt( a ) ) );
          c = 0.0;

          while ( z <= x ) {
            e = e * ( a / z );
            c = c + e;
            z += 1.0;
          }

          //              __chi2Cache.insert(conty,( c * y + s ));
          retVal = ( c * y + s );
        }
      } else {
        //            __chi2Cache.insert(conty,s);
        retVal = s;
      }
    }
    //      } else {
    //          ++nbChi;
    //        }
    //      std::cout << "Z avoid : " << nbZ << " / " << nbZt << ". Chi avoid :
    //      " << nbChi << " / " << nbChit << "." << std::endl;
    //      return __chi2Cache[conty];
    return retVal;
  }

}  // End of namespace gum

// HashTable<std::pair<double, unsigned long>, double> ChiSquare::__chi2Cache;
// HashTable<double, double> ChiSquare::__ZCache;
// Idx ChiSquare::nbZ = 0;
// Idx ChiSquare::nbChi = 0;
// Idx ChiSquare::nbZt = 0;
// Idx ChiSquare::nbChit = 0;
