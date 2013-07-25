/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
//#include <agrum/BN/io/BIF/cocoR/SyntaxBIF.atg>

#include <vector>
#include <numeric>

namespace std {


  template <typename T>
  ostream& operator<< ( ostream& stream, const vector<T>& val ) {
    bool deja = false;
    stream << "[";

    for ( typename vector<T>::const_iterator iter = val.begin();
          iter != val.end(); ++iter, deja=true ) {
      if ( deja ) stream << " , ";

      stream << *iter;
    }

    stream << "]";

    return stream;
  }

  template <typename T1, typename T2>
  ostream& operator<< ( ostream& stream, const pair<T1, T2>& val ) {
    stream << "(" << val.first << "," << val.second << ")";
    return stream;
  }


} /* namespace std */
namespace gum {

  /// return a random discrete distribution
  /// @param n is the number of modalities for the ditribution
  template<typename GUM_SCALAR>
  std::vector<GUM_SCALAR> randomDistribution( Size n ) {
    if ( n<2 ) n=2;

    std::vector<GUM_SCALAR> v( n );
    GUM_SCALAR s;

    do {
      for ( Idx i=0; i<n; i++ ) {
        v[i]=( GUM_SCALAR )randomProba();
      }

      s=std::accumulate( v.begin(),v.end(),( GUM_SCALAR )0.0 );

    } while ( s<( GUM_SCALAR )( 1e-5 ) );

    for ( Idx i=0; i<n; i++ ) {
      v[i]/=s;
    }

    return v;
  }

}


#endif  // DOXYGEN_SHOULD_SKIP_THIS
