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
/**
 * @file
 * @brief Definition of templatized reader of dat files for Factored Markov
 * Decision Process.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/FMDP/io/dat/fmdpDatReader.h>

namespace gum {

  template <typename GUM_SCALAR>
  FMDPDatReader<GUM_SCALAR>::FMDPDatReader( FMDP<GUM_SCALAR>*  fmdp,
                                            const std::string& filename )
      : FMDPReader<GUM_SCALAR>( fmdp, filename ) {

    GUM_CONSTRUCTOR( FMDPDatReader );

    __fmdp = fmdp;
    __streamName = filename;
    __parseDone = false;
    //    ddf->putOnNoVariableCheckMode();
    __factory = new FMDPFactory<GUM_SCALAR>( __fmdp );
    //~ __factory->setVerbose();
    __ioerror = false;

    try {
      __scanner = new MDPDAT::Scanner( __streamName.c_str() );
      __parser = new MDPDAT::Parser( __scanner );
      __parser->setFactory( (AbstractFMDPFactory*)__factory );
    } catch ( IOError e ) {
      __ioerror = true;
    }
  }

  template <typename GUM_SCALAR>
  FMDPDatReader<GUM_SCALAR>::~FMDPDatReader() {
    GUM_DESTRUCTOR( FMDPDatReader );

    if ( !__ioerror ) {
      // this could lead to memory leak !!
      if ( __parser ) delete ( __parser );

      if ( __scanner ) delete ( __scanner );
    }

    if ( __factory ) delete ( __factory );
  }

  template <typename GUM_SCALAR>
  INLINE MDPDAT::Scanner& FMDPDatReader<GUM_SCALAR>::scanner() {
    if ( __ioerror ) {
      GUM_ERROR( gum::IOError, "No such file " + streamName() );
    }

    return *__scanner;
  }

  template <typename GUM_SCALAR>
  INLINE const std::string& FMDPDatReader<GUM_SCALAR>::streamName() const {
    return __streamName;
  }

  template <typename GUM_SCALAR>
  INLINE bool FMDPDatReader<GUM_SCALAR>::trace( void ) const {
    return __traceScanning;
  }

  template <typename GUM_SCALAR>
  INLINE void FMDPDatReader<GUM_SCALAR>::trace( bool b ) {
    __traceScanning = b;
    scanner().setTrace( b );
  }

  template <typename GUM_SCALAR>
  Size FMDPDatReader<GUM_SCALAR>::proceed( void ) {
    if ( __ioerror ) {
      GUM_ERROR( gum::IOError, "No such file " + streamName() );
    }

    if ( !__parseDone ) {
      try {
        __parser->Parse();
      } catch ( gum::Exception& e ) {
        GUM_SHOWERROR( e );
        return 1 + __parser->errors().error_count;
      }

      __parseDone = true;
    }

    return ( __parser->errors().error_count );
  }

  /// @{
  /// publishing Errors API
  template <typename GUM_SCALAR>
  INLINE Idx FMDPDatReader<GUM_SCALAR>::errLine( Idx i ) {
    if ( __parseDone )
      return __parser->errors().error( i ).line;
    else
      GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template <typename GUM_SCALAR>
  INLINE Idx FMDPDatReader<GUM_SCALAR>::errCol( Idx i ) {
    if ( __parseDone )
      return __parser->errors().error( i ).column;
    else
      GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template <typename GUM_SCALAR>
  INLINE bool FMDPDatReader<GUM_SCALAR>::errIsError( Idx i ) {
    if ( __parseDone )
      return __parser->errors().error( i ).is_error;
    else
      GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template <typename GUM_SCALAR>
  INLINE std::string FMDPDatReader<GUM_SCALAR>::errMsg( Idx i ) {
    if ( __parseDone )
      return __parser->errors().error( i ).msg;
    else
      GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template <typename GUM_SCALAR>
  INLINE void FMDPDatReader<GUM_SCALAR>::showElegantErrors( std::ostream& o ) {
    if ( __parseDone )
      __parser->errors().elegantErrors( o );
    else
      GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template <typename GUM_SCALAR>
  INLINE void
  FMDPDatReader<GUM_SCALAR>::showElegantErrorsAndWarnings( std::ostream& o ) {
    if ( __parseDone )
      __parser->errors().elegantErrorsAndWarnings( o );
    else
      GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template <typename GUM_SCALAR>
  INLINE void FMDPDatReader<GUM_SCALAR>::showErrorCounts( std::ostream& o ) {
    if ( __parseDone )
      __parser->errors().syntheticResults( o );
    else
      GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template <typename GUM_SCALAR>
  INLINE Size FMDPDatReader<GUM_SCALAR>::errors() {
    return ( !__parseDone ) ? (Size)0 : __parser->errors().error_count;
  }

  template <typename GUM_SCALAR>
  INLINE Size FMDPDatReader<GUM_SCALAR>::warnings() {
    return ( !__parseDone ) ? (Size)0 : __parser->errors().warning_count;
  }

  /// @}
}  // namespace

#endif  // DOXYGEN_SHOULD_SKIP_THIS
