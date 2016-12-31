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
 * @brief Headers for the O3prmReader class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#ifndef GUM_PRM_O3PRM_O3PRM_FACTORY_H
#define GUM_PRM_O3PRM_O3PRM_FACTORY_H

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3ClassFactory.h>
#include <agrum/PRM/o3prm/O3InterfaceFactory.h>
#include <agrum/PRM/o3prm/O3NameSolver.h>
#include <agrum/PRM/o3prm/O3SystemFactory.h>
#include <agrum/PRM/o3prm/O3TypeFactory.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/cocoR/Parser.h>
#include <agrum/PRM/o3prm/cocoR/Scanner.h>
#include <agrum/core/errorsContainer.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      /**
       * @class O3prmReader
       * @headerfile O3prmReader.h <agrum/PRM/o3prm/O3prmReader.h>
       * @ingroup o3prm_group
       *
       * @brief This class read O3PRM files and creates the corresponding
       * gum::prm::PRM.
       *
       * @tparam GUM_SCALAR The scalar type to use with the gum::prm::PRM.
       */
      template <typename GUM_SCALAR>
      class O3prmReader {
        public:
        O3prmReader();
        O3prmReader( PRM<GUM_SCALAR>& prm );
        O3prmReader( const O3prmReader& src );
        O3prmReader( O3prmReader&& src );
        ~O3prmReader();
        O3prmReader& operator=( const O3prmReader& src );
        O3prmReader& operator=( O3prmReader&& src );

        /// Read file and load its content using a PRMFactory.
        /// The package parameter set the file's content package.
        Size readFile( const std::string& file, const std::string& module = "" );

        /// With readString method, you must set the current path
        /// to search from import yourself, using addClassPath.
        Size readString( const std::string& string );

        void parseStream( std::istream& input,
                          std::ostream& output,
                          std::string   module = "" );
        /**
         * @brief This methods defines the list of paths to look for o3prm
         * files.
         *
         * Use / for path separator ! Even on Windows !
         *
         * @param class_path A semicolon separated list of paths.
         */
        void setClassPath( const std::string& class_path );

        /**
         * @brief Add a list of paths to look for o3prm
         * files.
         *
         * Use / for path separator ! Even on Windows !
         *
         * @param class_path A semicolon separated list of paths.
         */
        void addClassPath( const std::string& class_path );

        gum::prm::PRM<GUM_SCALAR>*       prm() { return __prm; }
        const gum::prm::PRM<GUM_SCALAR>* prm() const { return __prm; }

        /// @{
        /// publishing Errors API

        /// # of errors
        Size errors() const;
        /// # of errors
        Size warnings() const;

        ///
        const ErrorsContainer& errorsContainer() const;

        /// line of ith error or warning
        Idx errLine( Idx i ) const;
        /// col of ith error or warning
        Idx errCol( Idx i ) const;
        /// filename of ith error or warning
        std::wstring errFilename( Idx i ) const;
        /// type of ith error or warning
        bool errIsError( Idx i ) const;
        /// message of ith error or warning
        std::string errMsg( Idx i ) const;

        /// send on std::cerr the list of errors
        void showElegantErrors( std::ostream& o = std::cerr ) const;

        /// send on std::cerr the list of errors or warnings
        void showElegantErrorsAndWarnings( std::ostream& o = std::cerr ) const;

        /// send on std::cerr the number of errors and the number of warnings
        void showErrorCounts( std::ostream& o = std::cerr ) const;
        /// @}

        private:
        PRM<GUM_SCALAR>*         __prm;
        std::unique_ptr<O3PRM>   __o3_prm;
        std::vector<std::string> __class_path;
        Set<std::string>         __imported;

        // Needed when file can't be parse (can not open it for exemple)
        ErrorsContainer __errors;

        void __readStream( std::istream&      input,
                           const std::string& file,
                           std::string        module = "" );

        void __parseImport( const O3Import& i, const std::string& module_path );

        void __parseStream( std::istream&      input,
                            const std::string& filename,
                            const std::string& module );

        std::vector<const O3Import*> __copyImports();

        std::string __clean( std::string text ) const;
        std::string __print( const ParseError& err ) const;
        std::string __readStream( std::istream& input );
      };

    }  // o3prm
  }    // prm
}  // gum

// always include the implementation of the templates
#include <agrum/PRM/o3prm/O3prmReader_tpl.h>


extern template class gum::prm::o3prm::O3prmReader<float>;
extern template class gum::prm::o3prm::O3prmReader<double>;


#endif  // GUM_PRM_O3PRM_O3PRM_FACTORY_H
