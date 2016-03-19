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

#include <agrum/BN/io/UAI/UAIBNReader.h>

namespace gum {

  template <typename GUM_SCALAR>
  UAIBNReader<GUM_SCALAR>::UAIBNReader( BayesUAIBN<GUM_SCALAR>* bn,
                                    const std::string& filename )
      : BNReader<GUM_SCALAR>( bn, filename ) {
    GUM_CONSTRUCTOR( UAIBNReader );
    __bn = bn;
    __streamName = filename;
    __parseDone = false;

    __factory = new BayesUAIBNFactory<GUM_SCALAR>( __bn );

    __ioerror = false;

    try {
      __scanner = new UAIBN::Scanner( __streamName.c_str() );
      __parser = new UAIBN::Parser( __scanner );
      __parser->setFactory( (IBayesUAIBNFactory*)__factory );
    } catch ( IOError e ) {
      __ioerror = true;
    }
  }

  template <typename GUM_SCALAR>
  UAIBNReader<GUM_SCALAR>::~UAIBNReader() {
    GUM_DESTRUCTOR( UAIBNReader );

    if ( !__ioerror ) {
      // this could lead to memory leak !!
      if ( __parser ) delete ( __parser );

      if ( __scanner ) delete ( __scanner );
    }

    if ( __factory ) delete ( __factory );
  }

  template <typename GUM_SCALAR>
  INLINE UAIBN::Scanner& UAIBNReader<GUM_SCALAR>::scanner() {
    if ( __ioerror ) {
      GUM_ERROR( gum::IOError, "No such file " + streamName() );
    }

    return *__scanner;
  }

  template <typename GUM_SCALAR>
  INLINE const std::string& UAIBNReader<GUM_SCALAR>::streamName() const {
    return __streamName;
  }

  template <typename GUM_SCALAR>
  INLINE bool UAIBNReader<GUM_SCALAR>::trace( void ) const {
    return __traceScanning;
  }

  template <typename GUM_SCALAR>
  INLINE void UAIBNReader<GUM_SCALAR>::trace( bool b ) {
    __traceScanning = b;
    scanner().setTrace( b );
  }

  template <typename GUM_SCALAR>
  int UAIBNReader<GUM_SCALAR>::proceed( void ) {
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

  // @{
  // publishing Errors API
  template <typename GUM_SCALAR>
  INLINE unsigned int UAIBNReader<GUM_SCALAR>::errLine( unsigned int i ) {
    if ( __parseDone )
      return __parser->errors().error( i ).line;
    else {
      GUM_ERROR( OperationNotAllowed, "UAIBN file not parsed yet" );
    }
  }

  template <typename GUM_SCALAR>
  INLINE unsigned int UAIBNReader<GUM_SCALAR>::errCol( unsigned int i ) {
    if ( __parseDone )
      return __parser->errors().error( i ).column;
    else {
      GUM_ERROR( OperationNotAllowed, "UAIBN file not parsed yet" );
    }
  }

  template <typename GUM_SCALAR>
  INLINE bool UAIBNReader<GUM_SCALAR>::errIsError( unsigned int i ) {
    if ( __parseDone )
      return __parser->errors().error( i ).is_error;
    else {
      GUM_ERROR( OperationNotAllowed, "UAIBN file not parsed yet" );
    }
  }

  template <typename GUM_SCALAR>
  INLINE std::string UAIBNReader<GUM_SCALAR>::errMsg( unsigned int i ) {
    if ( __parseDone )
      return __parser->errors().error( i ).msg;
    else {
      GUM_ERROR( OperationNotAllowed, "UAIBN file not parsed yet" );
    }
  }

  template <typename GUM_SCALAR>
  INLINE void UAIBNReader<GUM_SCALAR>::showElegantErrors( std::ostream& o ) {
    if ( __parseDone )
      __parser->errors().elegantErrors( o );
    else {
      GUM_ERROR( OperationNotAllowed, "UAIBN file not parsed yet" );
    }
  }

  template <typename GUM_SCALAR>
  INLINE void
  UAIBNReader<GUM_SCALAR>::showElegantErrorsAndWarnings( std::ostream& o ) {
    if ( __parseDone )
      __parser->errors().elegantErrorsAndWarnings( o );
    else {
      GUM_ERROR( OperationNotAllowed, "UAIBN file not parsed yet" );
    }
  }

  template <typename GUM_SCALAR>
  INLINE void UAIBNReader<GUM_SCALAR>::showErrorsAndWarnings( std::ostream& o ) {
    if ( __parseDone )
      __parser->errors().simpleErrorsAndWarnings( o );
    else {
      GUM_ERROR( OperationNotAllowed, "UAIBN file not parsed yet" );
    }
  }

  template <typename GUM_SCALAR>
  INLINE void UAIBNReader<GUM_SCALAR>::showErrorCounts( std::ostream& o ) {
    if ( __parseDone )
      __parser->errors().syntheticResults( o );
    else {
      GUM_ERROR( OperationNotAllowed, "UAIBN file not parsed yet" );
    }
  }

  template <typename GUM_SCALAR>
  INLINE Size UAIBNReader<GUM_SCALAR>::errors() {
    return ( !__parseDone ) ? (Size)0 : __parser->errors().error_count;
  }

  template <typename GUM_SCALAR>
  INLINE Size UAIBNReader<GUM_SCALAR>::warnings() {
    return ( !__parseDone ) ? (Size)0 : __parser->errors().warning_count;
  }

  // @}
}  // namespace

#endif  // DOXYGEN_SHOULD_SKIP_THIS
