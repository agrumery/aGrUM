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

#include <agrum/BN/io/UAI/UAIWriter.h>

namespace gum {

  /*
   * Default constructor.
   */
  template <typename GUM_SCALAR>
  INLINE UAIWriter<GUM_SCALAR>::UAIWriter() {
    GUM_CONSTRUCTOR( UAIWriter );
  }

  /*
   * Destructor.
   */
  template <typename GUM_SCALAR>
  INLINE UAIWriter<GUM_SCALAR>::~UAIWriter() {
    GUM_DESTRUCTOR( UAIWriter );
  }

  /*
   * Writes a bayes net in the given ouput stream.
   *
   * @param output The output stream.
   * @param bn The bayes net writen in the stream.
   * @throws IOError Raised if an I/O error occurs.
   */
  template <typename GUM_SCALAR>
  INLINE void UAIWriter<GUM_SCALAR>::write( std::ostream&                output,
                                            const IBayesNet<GUM_SCALAR>& bn ) {
    if ( !output.good() ) {
      GUM_ERROR( IOError, "Stream states flags are not all unset." );
    }

    output << __preambule( bn ) << std::endl;

    for ( auto node : bn.nodes() )
      output << __cptBloc( bn, node ) << std::endl;

    output << std::endl;

    output.flush();

    if ( output.fail() ) {
      GUM_ERROR( IOError, "Writing in the ostream failed." );
    }
  }

  /*
   * Writes a bayes net in the file referenced by filePath.
   * If the file doesn't exists, it is created.
   * If the file exists, it's content will be erased.
   *
   * @param filePath The path to the file used to write the bayes net.
   * @param bn The bayes net writen in the file.
   * @throw IOError Raised if an I/O error occurs.
   */
  template <typename GUM_SCALAR>
  INLINE void UAIWriter<GUM_SCALAR>::write( std::string                  filePath,
                                            const IBayesNet<GUM_SCALAR>& bn ) {
    std::ofstream output( filePath.c_str(), std::ios_base::trunc );

    write( output, bn );

    output.close();

    if ( output.fail() ) {
      GUM_ERROR( IOError, "Writing in the ostream failed." );
    }
  }

  template <typename GUM_SCALAR>
  INLINE std::string
  UAIWriter<GUM_SCALAR>::__preambule( const IBayesNet<GUM_SCALAR>& bn ) {
    std::stringstream str;

    str << "BAYES" << std::endl;

    str << bn.size() << std::endl;

    for ( auto node : bn.nodes() )
      str << bn.variable( node ).domainSize() << " ";
    str << std::endl;

    str << bn.size() << std::endl;  // number of potentials

    for ( auto node : bn.nodes() ) {
      const auto& p = bn.cpt( node );
      str << p.nbrDim() << " ";
      for ( auto k : p.variablesSequence() ) {
        str << bn.idFromName( k->name() ) << " ";
      }
      str << "  # " << bn.variable( node ).name() << std::endl;
    }
    str << std::endl;

    return str.str();
  }
  template <typename GUM_SCALAR>
  INLINE std::string
  UAIWriter<GUM_SCALAR>::__cptBloc( const IBayesNet<GUM_SCALAR>& bn,
                                    NodeId                       node ) {
    std::stringstream str;

    const auto& p = bn.cpt( node );
    str << p.domainSize();
    Instantiation I( p );
    for ( I.setFirst(); !I.end(); ++I ) {
      if ( I.val( 0 ) == 0 ) str << std::endl << "  ";
      str << p[I] << " ";
    }
    str << std::endl;

    return str.str();
  }

} /* namespace gum */

#endif  // DOXYGEN_SHOULD_SKIP_THIS
