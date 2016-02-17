/**************************************************************************
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
 * @brief Implementation for the O3PRMFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/newo3prm/O3PRMFactory.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

#include <agrum/PRM/newo3prm/o3prm.h>
#include <agrum/PRM/newo3prm/utils.h>
#include <agrum/PRM/newo3prm/O3SystemFactory.h>
#include <agrum/PRM/newo3prm/O3TypeFactory.h>
#include <agrum/PRM/newO3prm/cocoR/Parser.h>
#include <agrum/PRM/newO3prm/cocoR/Scanner.h>
#include <agrum/PRM/newo3prm/O3ClassFactory.h>
#include <agrum/PRM/newo3prm/O3InterfaceFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      using o3prm_scanner = gum::prm::newo3prm::Scanner;
      using o3prm_parser = gum::prm::newo3prm::Parser;

      template <typename INLINE GUM_SCALAR>
      O3PRMFactory<GUM_SCALAR>::O3PRMFactory( PRM<GUM_SCALAR>& prm )
          : __prm( &prm )
      {
        GUM_CONSTRUCTOR( O3PRMFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3PRMFactory<GUM_SCALAR>::O3PRMFactory( const O3PRMFactory& src )
          : __prm( src.__prm )
          , __class_path( src.__class_path )
          , __errors( src.__errors ) {
        GUM_CONS_CPY( O3PRMFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3PRMFactory<GUM_SCALAR>::O3PRMFactory( O3PRMFactory&& src )
          : __prm( std::move( src.__prm ) )
          , __class_path( std::move( src.__class_path ) )
          , __errors( std::move( src.__errors ) ) {
        GUM_CONS_CPY( O3PRMFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3PRMFactory<GUM_SCALAR>::~O3PRMFactory() {
        GUM_DESTRUCTOR( O3PRMFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3PRMFactory<GUM_SCALAR>& O3PRMFactory<GUM_SCALAR>::
      operator=( const O3PRMFactory& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = src.__prm;
        __class_path = src.__class_path;
        __errors = src.__errors;
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE O3PRMFactory<GUM_SCALAR>& O3PRMFactory<GUM_SCALAR>::
      operator=( O3PRMFactory&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = std::move( src.__prm );
        __class_path = std::move( src.__class_path );
        __errors = std::move( src.__errors );
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE int
      O3PRMFactory<GUM_SCALAR>::readFile( const std::string& file,
                                          const std::string& package ) {
        return 0;
      }

      template <typename GUM_SCALAR>
      INLINE int
      O3PRMFactory<GUM_SCALAR>::readString( const std::string& str ) {
        return 0;
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3PRMFactory<GUM_SCALAR>::setClassPath( const std::string& class_path ) {}

      template <typename GUM_SCALAR>
      INLINE void
      O3PRMFactory<GUM_SCALAR>::addClassPath( const std::string& class_path ) {}

      template <typename GUM_SCALAR>
      INLINE unsigned int
      O3PRMFactory<GUM_SCALAR>::errLine( unsigned int i ) const {
        return __errors.error( i ).line;
      }

      template <typename GUM_SCALAR>
      INLINE unsigned int
      O3PRMFactory<GUM_SCALAR>::errCol( unsigned int i ) const {
        return __errors.error( i ).column;
      }

      template <typename GUM_SCALAR>
      INLINE std::wstring
      O3PRMFactory<GUM_SCALAR>::errFilename( unsigned int i ) const {
        return widen( __errors.error( i ).filename );
      }

      template <typename GUM_SCALAR>
      INLINE bool O3PRMFactory<GUM_SCALAR>::errIsError( unsigned int i ) const {
        return __errors.error( i ).is_error;
      }

      template <typename GUM_SCALAR>
      INLINE std::string
      O3PRMFactory<GUM_SCALAR>::errMsg( unsigned int i ) const {
        return __errors.error( i ).msg;
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3PRMFactory<GUM_SCALAR>::showElegantErrors( std::ostream& o ) const {
        __errors.elegantErrors( o );
      }

      template <typename GUM_SCALAR>
      INLINE void O3PRMFactory<GUM_SCALAR>::showElegantErrorsAndWarnings(
          std::ostream& o ) const {
        __errors.elegantErrorsAndWarnings( o );
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3PRMFactory<GUM_SCALAR>::showErrorCounts( std::ostream& o ) const {
        __errors.syntheticResults( o );
      }

      template <typename GUM_SCALAR>
      INLINE Size O3PRMFactory<GUM_SCALAR>::errors() const {
        return __errors.error_count;
      }

      template <typename GUM_SCALAR>
      INLINE Size O3PRMFactory<GUM_SCALAR>::warnings() const {
        return __errors.warning_count;
      }

      template <typename GUM_SCALAR>
      INLINE const ErrorsContainer&
      O3PRMFactory<GUM_SCALAR>::errorsContainer() const {
        return __errors;
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3PRMFactory<GUM_SCALAR>::parseStream( std::istream& input,
                                             std::ostream& output ) {
        __readStream( input );

        for ( auto i = 0; i < __errors.count(); ++i ) {
          auto err = __errors.error( i );
          output << print( err ) << std::endl;
        }
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3PRMFactory<GUM_SCALAR>::__readStream( std::istream& input ) {
        auto sBuff = read_stream( input );
        auto buffer = std::unique_ptr<unsigned char[]>(
            new unsigned char[sBuff.length() + 1] );
        strcpy( (char*)buffer.get(), sBuff.c_str() );
        auto s = o3prm_scanner( buffer.get(), sBuff.length() + 1, "" );
        auto p = o3prm_parser( &s );
        auto o3_prm = gum::prm::o3prm::O3PRM();
        p.set_prm( &o3_prm );
        p.Parse();
        __errors = p.errors();

        auto output = std::stringstream();
        if ( __errors.error_count == 0 ) {
          auto type_factory = O3TypeFactory<GUM_SCALAR>();

          auto interface_factory = O3InterfaceFactory<GUM_SCALAR>();
          auto class_factory = O3ClassFactory<GUM_SCALAR>();
          auto system_factory = O3SystemFactory<GUM_SCALAR>();

          type_factory.build( *__prm, o3_prm, __errors );
          interface_factory.buildInterfaces( *__prm, o3_prm, __errors );
          class_factory.buildClasses( *__prm, o3_prm, __errors );
          interface_factory.buildElements( *__prm, o3_prm, __errors );
          class_factory.buildImplementations( *__prm, o3_prm, __errors );
          class_factory.buildParameters( *__prm, o3_prm, __errors );
          class_factory.buildReferenceSlots( *__prm, o3_prm, __errors );
          class_factory.declareAttributes( *__prm, o3_prm, __errors );
          class_factory.buildAggregates( *__prm, o3_prm, __errors );
          class_factory.completeAttributes( *__prm, o3_prm, __errors );
          system_factory.build( *__prm, o3_prm, __errors );
        }
      }
    }
  }
}
