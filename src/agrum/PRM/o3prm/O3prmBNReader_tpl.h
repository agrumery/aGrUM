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
 * @brief Inline implementation of O3prmReader.
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI
 */

// to ease Parser
#include <agrum/PRM/o3prm/O3prmBNReader.h>

namespace gum {
  template <typename GUM_SCALAR>
  INLINE std::string O3prmBNReader<GUM_SCALAR>::__getVariableName(
      const std::string& path, const std::string& type, const std::string& name ) {
    return path + name;  // path ends up with a "."
  }

  template <typename GUM_SCALAR>
  std::string
  O3prmBNReader<GUM_SCALAR>::__getInstanceName( const std::string& classname ) {
    auto res = classname.substr( 0, 4 );
    std::transform( res.begin(), res.end(), res.begin(), ::tolower );
    return res;
  }

  template <typename GUM_SCALAR>
  std::string
  O3prmBNReader<GUM_SCALAR>::__getEntityName( const std::string& filename ) {
    auto b = filename.find_last_of( "/\\" );
    auto e = filename.find_last_of( "." ) - 1;
    GUM_ASSERT( e > b );  // we are waiting ../../basename.o3prm
    return filename.substr( b + 1, e - b );
  }

  template <typename GUM_SCALAR>
  O3prmBNReader<GUM_SCALAR>::O3prmBNReader( BayesNet<GUM_SCALAR>* bn,
                                            const std::string&    filename,
                                            const std::string&    entityName,
                                            const std::string&    classpath )
      : BNReader<GUM_SCALAR>( bn, filename ) {
    GUM_CONSTRUCTOR( O3prmBNReader );
    __bn = bn;
    __filename = filename;
    __entityName = entityName == "" ? __getEntityName( filename ) : entityName;
    __classpath = classpath;
  }

  template <typename GUM_SCALAR>
  O3prmBNReader<GUM_SCALAR>::~O3prmBNReader() {
    GUM_DESTRUCTOR( O3prmBNReader );
  }

  /// parse.
  /// @return the number of detected errors
  /// @throws IOError if file not exists
  template <typename GUM_SCALAR>
  Size O3prmBNReader<GUM_SCALAR>::proceed( void ) {
    prm::o3prm::O3prmReader<GUM_SCALAR> reader;
    if ( __classpath != "" ) {
      reader.addClassPath( __classpath );
    }
    reader.readFile( __filename );
    gum::prm::PRM<GUM_SCALAR>* prm = reader.prm();
    __errors = reader.errorsContainer();

    if ( errors() == 0 ) {
      if ( prm->isSystem( __entityName ) ) {
        __generateBN( prm->getSystem( __entityName ) );
      } else {
        if ( prm->isClass( __entityName ) ) {
          ParseError warn( false,
                           "No system '" + __entityName +
                               "' found but class found. Generating instance.",
                           __filename,
                           0 );
          __errors.add( warn );
          gum::prm::PRMSystem<GUM_SCALAR> s( "S_" + __entityName );
          auto i = new gum::prm::PRMInstance<GUM_SCALAR>(
              __getInstanceName( __entityName ), prm->getClass( __entityName ) );
          s.add( i );
          __generateBN( s );
        } else {
          ParseError err( true,
                          "Neither system nor class '" + __entityName + "'.",
                          __filename,
                          0 );
          __errors.add( err );
        }
      }

      // renaming varible in th BN

      std::regex            re( "([^\\(]+)(\\([^\\)]+\\))(.*)" );
      std::smatch           match;
      gum::Set<std::string> names;
      for ( auto node : __bn->nodes() ) {
        // keeping the complete name in description
        const std::string& nn = __bn->variable( node ).name();
        __bn->variable( node ).setDescription( nn );

        // trying to simplify the name
        if ( std::regex_search( nn, match, re ) ) {
          if ( match.size() != 4 ) {
            ParseError warn(
                false, "Name " + nn + " cannot be simplified.", __filename, 0 );
            __errors.add( warn );
          } else {
            std::string newNameRadical = __getVariableName(
                match.str( 1 ), match.str( 2 ), match.str( 3 ) );
            std::string newName = newNameRadical;
            // forcing newName to be unique
            int num = 0;
            while ( names.contains( newName ) ) {
              newName = newNameRadical + std::to_string( ++num );
            }

            names.insert( newName );
            __bn->changeVariableName( node, newName );
          }
        }
      }
    }

    delete prm;

    return errors();
  }


  template <typename GUM_SCALAR>
  void
  O3prmBNReader<GUM_SCALAR>::__generateBN( prm::PRMSystem<GUM_SCALAR>& system ) {
    system.instantiate();
    BayesNetFactory<GUM_SCALAR> factory( __bn );
    system.groundedBN( factory );
  }
}  // gum
