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
 * @brief Implementation for the O3prmReader class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/config.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      INLINE std::string
      O3prmReader<GUM_SCALAR>::__clean( std::string text ) const {
        auto match = std::regex( "Syntax error" );
        if ( std::regex_search( text, match ) ) {
          text = std::regex_replace( text, match, std::string("Error") );
          auto regex = std::regex{"[A-Z_][A-Z_]+"};
          text = std::regex_replace( text, regex, std::string("declaration") );
          return text;
        }
        return text;
      }

      template <typename GUM_SCALAR>
      INLINE std::string
      O3prmReader<GUM_SCALAR>::__print( const ParseError& err ) const {
        std::stringstream s;
        s << err.filename << "|" << err.line << " col " << err.column << "| "
          << __clean( err.msg );
        return s.str();
      }

      template <typename GUM_SCALAR>
      INLINE std::string
      O3prmReader<GUM_SCALAR>::__readStream( std::istream& inputstr ) {
        if ( inputstr ) {
          inputstr.seekg( 0, inputstr.end );
          int length = int( inputstr.tellg() );
          inputstr.seekg( 0, inputstr.beg );

          auto str = std::string();
          str.resize( length, ' ' );
          auto begin = &*str.begin();

          inputstr.read( begin, length );

          return str;
        }
        GUM_ERROR( OperationNotAllowed, "Could not open file" );
      }

      using o3prm_scanner = gum::prm::o3prm::Scanner;
      using o3prm_parser = gum::prm::o3prm::Parser;

      template <typename GUM_SCALAR>
      INLINE O3prmReader<GUM_SCALAR>::O3prmReader()
          : __prm( new PRM<GUM_SCALAR>() )
          , __o3_prm( std::unique_ptr<O3PRM>( new O3PRM() ) ) {
        GUM_CONSTRUCTOR( O3prmReader );
      }

      template <typename GUM_SCALAR>
      INLINE O3prmReader<GUM_SCALAR>::O3prmReader( PRM<GUM_SCALAR>& prm )
          : __prm( &prm )
          , __o3_prm( std::unique_ptr<O3PRM>( new O3PRM() ) ) {
        GUM_CONSTRUCTOR( O3prmReader );
      }

      template <typename GUM_SCALAR>
      INLINE O3prmReader<GUM_SCALAR>::O3prmReader( const O3prmReader& src )
          : __prm( src.__prm )
          , __o3_prm( std::unique_ptr<O3PRM>( new O3PRM( *( src.__o3_prm ) ) ) )
          , __class_path( src.__class_path )
          , __imported( src.__imported )
          , __errors( src.__errors ) {
        GUM_CONS_CPY( O3prmReader );
      }

      template <typename GUM_SCALAR>
      INLINE O3prmReader<GUM_SCALAR>::O3prmReader( O3prmReader&& src )
          : __prm( std::move( src.__prm ) )
          , __o3_prm( std::move( src.__o3_prm ) )
          , __class_path( std::move( src.__class_path ) )
          , __imported( std::move( src.__imported ) )
          , __errors( std::move( src.__errors ) ) {
        GUM_CONS_CPY( O3prmReader );
      }

      template <typename GUM_SCALAR>
      INLINE O3prmReader<GUM_SCALAR>::~O3prmReader() {
        GUM_DESTRUCTOR( O3prmReader );
      }

      template <typename GUM_SCALAR>
      INLINE O3prmReader<GUM_SCALAR>& O3prmReader<GUM_SCALAR>::
      operator=( const O3prmReader& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = src.__prm;
        __o3_prm = std::unique_ptr<O3PRM>( new O3PRM( *( src.__o3_prm ) ) );
        __class_path = src.__class_path;
        __imported = src.__imported;
        __errors = src.__errors;
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE O3prmReader<GUM_SCALAR>& O3prmReader<GUM_SCALAR>::
      operator=( O3prmReader&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = std::move( src.__prm );
        __o3_prm = std::move( src.__o3_prm );
        __class_path = std::move( src.__class_path );
        __imported = std::move( src.__imported );
        __errors = std::move( src.__errors );
        return *this;
      }

      template <typename GUM_SCALAR>
      void O3prmReader<GUM_SCALAR>::setClassPath( const std::string& class_path ) {
        __class_path = std::vector<std::string>();
        size_t i = 0;
        size_t j = class_path.find( ';' );

        while ( j != std::string::npos ) {
          addClassPath( class_path.substr( i, j - i ) );
          i = j + 1;

          if ( i < class_path.size() ) {
            j = class_path.find( ';', i );
          } else {
            j = std::string::npos;
          }
        }

        if ( i < class_path.size() ) {
          addClassPath( class_path.substr( i, std::string::npos ) );
        }
      }

      template <typename GUM_SCALAR>
      void O3prmReader<GUM_SCALAR>::addClassPath( const std::string& class_path ) {
        auto path = class_path;
        if ( path[path.size() - 1] != '/' ) {
          path.append( "/" );
        }
        auto dir = Directory( path );

        if ( !dir.isValid() ) {
          __errors.addException( "could not resolve class path", path );
        } else {
          __class_path.push_back( std::move( path ) );
        }
      }

      template <typename GUM_SCALAR>
      INLINE Idx O3prmReader<GUM_SCALAR>::errLine( Idx i ) const {
        return __errors.error( i ).line;
      }

      template <typename GUM_SCALAR>
      INLINE Idx O3prmReader<GUM_SCALAR>::errCol( Idx i ) const {
        return __errors.error( i ).column;
      }

      template <typename GUM_SCALAR>
      INLINE std::wstring O3prmReader<GUM_SCALAR>::errFilename( Idx i ) const {
        return widen( __errors.error( i ).filename );
      }

      template <typename GUM_SCALAR>
      INLINE bool O3prmReader<GUM_SCALAR>::errIsError( Idx i ) const {
        return __errors.error( i ).is_error;
      }

      template <typename GUM_SCALAR>
      INLINE std::string O3prmReader<GUM_SCALAR>::errMsg( Idx i ) const {
        return __errors.error( i ).msg;
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3prmReader<GUM_SCALAR>::showElegantErrors( std::ostream& o ) const {
        for ( Idx i = 0; i < __errors.count(); ++i ) {
          auto err = __errors.error( i );
          o << __print( err ) << std::endl;
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3prmReader<GUM_SCALAR>::showElegantErrorsAndWarnings(
          std::ostream& o ) const {
        for ( Idx i = 0; i < __errors.count(); ++i ) {
          auto err = __errors.error( i );
          o << __print( err ) << std::endl;
        }
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3prmReader<GUM_SCALAR>::showErrorCounts( std::ostream& o ) const {
        __errors.syntheticResults( o );
      }

      template <typename GUM_SCALAR>
      INLINE Size O3prmReader<GUM_SCALAR>::errors() const {
        return __errors.error_count;
      }

      template <typename GUM_SCALAR>
      INLINE Size O3prmReader<GUM_SCALAR>::warnings() const {
        return __errors.warning_count;
      }

      template <typename GUM_SCALAR>
      INLINE const ErrorsContainer&
                   O3prmReader<GUM_SCALAR>::errorsContainer() const {
        return __errors;
      }

      template <typename GUM_SCALAR>
      INLINE Size O3prmReader<GUM_SCALAR>::readString( const std::string& str ) {
        std::stringstream sBuff( str );
        __readStream( sBuff, "" );
        return __errors.count();
      }

      template <typename GUM_SCALAR>
      INLINE Size O3prmReader<GUM_SCALAR>::readFile( const std::string& file,
                                                     const std::string& module ) {
        try {
          auto lastSlashIndex = file.find_last_of( '/' );

          auto dir = Directory( file.substr( 0, lastSlashIndex + 1 ) );

          if ( !dir.isValid() ) {
            __errors.addException( "could not find file", file );
            return __errors.count();
          }

          auto basename = file.substr( lastSlashIndex + 1 );
          auto absFilename = dir.absolutePath() + basename;

          std::ifstream input( absFilename );
          if ( input.is_open() ) {
            __readStream( input, file, module );
          } else {
            __errors.addException( "could not open file", file );
          }

          return __errors.count();

        } catch ( gum::Exception& e ) {
          GUM_SHOWERROR( e );
          __errors.addException( e.errorContent(), file );
          return __errors.count();
        } catch ( ... ) {
          __errors.addException( "unknown error", file );
          return __errors.count();
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3prmReader<GUM_SCALAR>::parseStream( std::istream& input,
                                                        std::ostream& output,
                                                        std::string   module ) {
        __readStream( input, "", module );

        showElegantErrorsAndWarnings( output );
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3prmReader<GUM_SCALAR>::__parseStream( std::istream&      input,
                                              const std::string& filename,
                                              const std::string& module ) {
        auto sBuff = __readStream( input );
        auto buffer = std::unique_ptr<unsigned char[]>(
            new unsigned char[sBuff.length() + 1] );
        strcpy( (char*)buffer.get(), sBuff.c_str() );
        auto s =
            o3prm_scanner( buffer.get(), int( sBuff.length() + 1 ), filename );
        auto p = o3prm_parser( &s );
        p.set_prm( __o3_prm.get() );
        p.set_prefix( module );
        p.Parse();
        __errors += p.errors();
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3prmReader<GUM_SCALAR>::__parseImport( const O3Import&    i,
                                              const std::string& module ) {

        if ( !__imported.exists( i.import().label() ) ) {
          __imported.insert( i.import().label() );

          auto module_path = module;
          std::replace( module_path.begin(), module_path.end(), '.', '/' );

          auto path = i.import().label();
          std::replace( path.begin(), path.end(), '.', '/' );

          auto imported = false;
          for ( const auto& cp : __class_path ) {

            auto          file_path = cp + path + ".o3prm";
            std::ifstream file( file_path );

            if ( file.is_open() ) {

              __parseStream( file, file_path, i.import().label() );
              imported = true;
              break;
            }

            file_path = cp + module + path + ".o3prm";
            std::ifstream file2( file_path );

            if ( file2.is_open() ) {

              __parseStream( file2, file_path, module + "." + i.import().label() );
              imported = true;
              break;
            }
          }

          if ( !imported ) {
            const auto&       pos = i.import().position();
            std::stringstream msg;
            msg << "Import error: could not resolve import " << i.import().label();
            __errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
          }
        }
      }

      template <typename GUM_SCALAR>
      INLINE std::vector<const O3Import*>
             O3prmReader<GUM_SCALAR>::__copyImports() {
        auto copy = std::vector<const O3Import*>();
        for ( const auto& i : __o3_prm->imports() ) {
          if ( !__imported.exists( i->import().label() ) ) {
            copy.push_back( i.get() );
          }
        }
        return copy;
      }

      template <typename GUM_SCALAR>
      INLINE void O3prmReader<GUM_SCALAR>::__readStream( std::istream&      input,
                                                         const std::string& file,
                                                         std::string module ) {

        if ( module.size() > 0 && module.back() != '.' ) {
          module.append( "." );
        }

        __parseStream( input, file, module );

        auto imports = __copyImports();
        do {
          for ( auto i : imports ) {
            __parseImport( *i, module );
          }
          imports = __copyImports();
        } while ( imports.size() > 0 );


        if ( __errors.error_count == 0 ) {
          auto solver = O3NameSolver<GUM_SCALAR>( *__prm, *__o3_prm, __errors );
          auto type_factory =
              O3TypeFactory<GUM_SCALAR>( *__prm, *__o3_prm, solver, __errors );

          auto interface_factory = O3InterfaceFactory<GUM_SCALAR>(
              *__prm, *__o3_prm, solver, __errors );
          auto class_factory =
              O3ClassFactory<GUM_SCALAR>( *__prm, *__o3_prm, solver, __errors );

          auto system_factory =
              O3SystemFactory<GUM_SCALAR>( *__prm, *__o3_prm, solver, __errors );

          try {
            type_factory.build();
            interface_factory.buildInterfaces();
            class_factory.buildClasses();
            interface_factory.buildElements();
            class_factory.buildImplementations();
            class_factory.buildParameters();
            class_factory.buildReferenceSlots();
            class_factory.declareAttributes();
            class_factory.declareAggregates();
            class_factory.completeAggregates();
            class_factory.completeAttributes();
            system_factory.build();
          } catch ( Exception& ) {

            if ( __errors.count() == 0 ) {
              __errors.addException( "an unknown error occured", file );
            }
            // GUM_TRACE_NEWLINE;
            // GUM_SHOWERROR( e );

          } catch ( ... ) {
            __errors.addException( "an unknown exception occured", file );
          }
        }
      }
    }
  }
}
