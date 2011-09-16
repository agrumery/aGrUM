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
 * @brief Definition of templatized reader of dat files for Factored Markov Decision Process.
 *
 * @author Jean-Christophe Magnan
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/FMDP/io/dat/FMDPDatReader.h>

namespace gum {

  template<typename T_DATA>
  FMDPDatReader<T_DATA>::FMDPDatReader( FactoredMarkovDecisionProcess<T_DATA>* fmdp, const std::string& filename, MultiDimDecisionDiagramFactoryBase<T_DATA>* ddf ) 
			: FMDPReader<T_DATA> ( fmdp, filename ) {
				
    GUM_CONSTRUCTOR( FMDPDatReader );
    
    __fmdp = fmdp;
    __streamName = filename;
    __parseDone = false;
	ddf->putOnNoVariableCheckMode();
    __factory = new FMDPFactory<T_DATA> ( __fmdp, ddf );
	//~ __factory->setVerbose();
    __ioerror = false;
    
    try {
		__scanner = new MDPDAT::Scanner( __streamName.c_str() );
		__parser = new MDPDAT::Parser( __scanner );
		__parser->setFactory( (AbstractFMDPFactory*)__factory );
    } catch ( IOError e ) {
		__ioerror=true;
    }
  }

  template<typename T_DATA>
  FMDPDatReader<T_DATA>::~FMDPDatReader() {
    GUM_DESTRUCTOR( FMDPDatReader );

    if ( !__ioerror ) {
			// this could lead to memory leak !!
      if ( __parser ) delete( __parser );

      if ( __scanner ) delete( __scanner );
    }

    if ( __factory ) delete( __factory );
  }

  template<typename T_DATA> INLINE
  MDPDAT::Scanner& FMDPDatReader<T_DATA>::scanner() {
    if ( __ioerror ) {
      GUM_ERROR( gum::IOError, "No such file " + streamName() );
    }

    return *__scanner;
  }

  template<typename T_DATA> INLINE
  const std::string& FMDPDatReader<T_DATA>::streamName() const {
    return __streamName;
  }

  template<typename T_DATA> INLINE
  bool FMDPDatReader<T_DATA>::trace( void ) const {
    return __traceScanning;
  }

  template<typename T_DATA> INLINE
  void FMDPDatReader<T_DATA>::trace( bool b ) {
    __traceScanning=b;
    scanner().setTrace( b );
  }

  template<typename T_DATA>
  int FMDPDatReader<T_DATA>::proceed( void ) {
    if ( __ioerror ) {
      GUM_ERROR( gum::IOError,"No such file " + streamName() );
    }
    
    if ( !__parseDone ) {
      try {
        __parser->Parse();
      } catch ( gum::Exception &e ) {
        GUM_SHOWERROR( e );
        return 1 + __parser->errors().count();
      }

      __parseDone=true;
    }

    return (__parser->errors().error_count==0);
  }

  /// @{
  /// publishing Errors API
  template<typename T_DATA> INLINE
  unsigned int FMDPDatReader<T_DATA>::errLine( unsigned int i ) {
    if ( __parseDone ) return __parser->errors().line( i );
    else GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template<typename T_DATA> INLINE
  unsigned int FMDPDatReader<T_DATA>::errCol( unsigned int i ) {
    if ( __parseDone ) return __parser->errors().col( i );
    else GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template<typename T_DATA> INLINE
  bool FMDPDatReader<T_DATA>::errIsError( unsigned int i ) {
    if ( __parseDone ) return __parser->errors().is_error( i );
    else GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template<typename T_DATA> INLINE
  std::string FMDPDatReader<T_DATA>::errMsg( unsigned int i ) {
    if ( __parseDone ) return std::string( narrow( __parser->errors().msg( i ) ) );
    else GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template<typename T_DATA> INLINE
  void FMDPDatReader<T_DATA>::showElegantErrors() {
    if ( __parseDone ) __parser->errors().showElegantErrors();
    else GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template<typename T_DATA> INLINE
  void FMDPDatReader<T_DATA>::showElegantErrorsAndWarnings() {
    if ( __parseDone ) __parser->errors().showElegantErrorsAndWarnings();
    else GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template<typename T_DATA> INLINE
  void FMDPDatReader<T_DATA>::showErrorCounts() {
    if ( __parseDone ) __parser->errors().showSyntheticResults();
    else GUM_ERROR( OperationNotAllowed, "FMDPDat file not parsed yet" );
  }

  template<typename T_DATA> INLINE
  Size  FMDPDatReader<T_DATA>::errors() {
    return (! __parseDone )?( Size )0:__parser->errors().error_count;
  }

  template<typename T_DATA> INLINE
  Size  FMDPDatReader<T_DATA>::warnings() {
    return (! __parseDone )?( Size )0:__parser->errors().warning_count;
  }

  /// @}
} // namespace

#endif //DOXYGEN_SHOULD_SKIP_THIS

