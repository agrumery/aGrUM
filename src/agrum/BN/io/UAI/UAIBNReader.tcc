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
#include <iostream>
#include <fstream>

#include <agrum/BN/io/UAI/UAIBNReader.h>

namespace gum {
  /*
   * Constructor
   * A reader is created to reading a defined file.
   * Note that an BN as to be created before and given in parameter.
   */
  template <typename GUM_SCALAR>
  INLINE UAIBNReader<GUM_SCALAR>::UAIBNReader( BayesNet<GUM_SCALAR>* bn,
                                               const std::string& filePath )
      : BNReader<GUM_SCALAR>( bn, filePath ) {
    GUM_CONSTRUCTOR( UAIBNReader );
    __bn = bn;
    __filePath = filePath;
  }

  /*
   * Default destructor.
   */
  template <typename GUM_SCALAR>
  INLINE UAIBNReader<GUM_SCALAR>::~UAIBNReader() {
    GUM_DESTRUCTOR( UAIBNReader );
  }

  /*
   * Reads the bayes net from the file referenced by filePath  given at the
   * creation
   * of class
   * @return Returns the number of error during the parsing (0 if none).
   */
  template <typename GUM_SCALAR>
  int UAIBNReader<GUM_SCALAR>::proceed() {
    try {
      std::ifstream uaifile( __filePath, std::ios_base::in );

      // Loading file
      GUM_EMIT2( onProceed, 0, "Verifying header" );
      std::string label;
      uaifile >> label;
      if ( label != "BAYES" )
        __errors.addWarning(
            "<" + label + "> is not recognized", __filePath, 1, 1 );
      GUM_TRACE_VAR( label );

      GUM_EMIT2( onProceed, 1, "Verifying number of variables" );
      int val;
      uaifile >> val;
      if
      GUM_TRACE_VAR(val);

    } catch ( gum::IOError& e ) {
      GUM_ERROR( IOError, "Error(s)" );
      return 1;
    }
    return __errors.error_count;
  }

} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
