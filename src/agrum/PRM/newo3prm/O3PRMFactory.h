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
 * @brief Implementation for the O3TypeFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#ifndef GUM_PRM_O3PRM_O3PRM_FACTORY_H
#define GUM_PRM_O3PRM_O3PRM_FACTORY_H

#include <string>
#include <vector>

#include <agrum/core/errorsContainer.h>
#include <agrum/PRM/PRM.h>
#include <agrum/PRM/newo3prm/O3PRM.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      class O3PRMFactory {
        public:
        O3PRMFactory( );
        O3PRMFactory( PRM<GUM_SCALAR>& prm );
        O3PRMFactory( const O3PRMFactory& src );
        O3PRMFactory( O3PRMFactory&& src );
        ~O3PRMFactory();
        O3PRMFactory& operator=( const O3PRMFactory& src );
        O3PRMFactory& operator=( O3PRMFactory&& src );

        /// Read file and load its content using a PRMFactory.
        /// The package parameter set the file's content package.
        int readFile( const std::string& file,
                      const std::string& module = "" );

        /// With readString method, you must set the current path
        /// to search from import yourself, using addClassPath.
        int readString( const std::string& string );

        void parseStream( std::istream& input,
                          std::ostream& output,
                          std::string module = "" );
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

        gum::prm::PRM<GUM_SCALAR>* prm() { return __prm; }
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
        unsigned int errLine( unsigned int i ) const;
        /// col of ith error or warning
        unsigned int errCol( unsigned int i ) const;
        /// filename of ith error or warning
        std::wstring errFilename( unsigned int i ) const;
        /// type of ith error or warning
        bool errIsError( unsigned int i ) const;
        /// message of ith error or warning
        std::string errMsg( unsigned int i ) const;

        /// send on std::cerr the list of errors
        void showElegantErrors( std::ostream& o = std::cerr ) const;

        /// send on std::cerr the list of errors or warnings
        void showElegantErrorsAndWarnings( std::ostream& o = std::cerr ) const;

        /// send on std::cerr the number of errors and the number of warnings
        void showErrorCounts( std::ostream& o = std::cerr ) const;
        /// @}

        private:
        PRM<GUM_SCALAR>* __prm;
        std::unique_ptr<O3PRM> __o3_prm;
        std::vector<std::string> __class_path;
        Set<std::string> __imported;

        // Needed when file can't be parse (can not open it for exemple)
        ErrorsContainer __errors;

        // Read a file into a std::string
        std::string __readFile( const std::string& file );

        void __readStream( std::istream& input,
                           const std::string& file,
                           std::string module = "" );

        void __parseImport( const O3Import& i, const std::string& module_path );

        void __parseStream( std::istream& input,
                            const std::string& filename,
                            const std::string& module );
      };

    } // o3prm
  } // prm
} // gum

#include <agrum/PRM/newo3prm/O3PRMFactory.tcc>

extern template class gum::prm::o3prm::O3PRMFactory<float>;
extern template class gum::prm::o3prm::O3PRMFactory<double>;

#endif // GUM_PRM_O3PRM_O3PRM_FACTORY_H

