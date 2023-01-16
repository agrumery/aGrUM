/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers for the O3prmReader class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
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

#include <agrum/PRM/o3prm/O3ClassFactory.h>
#include <agrum/PRM/o3prm/O3InterfaceFactory.h>
#include <agrum/PRM/o3prm/O3SystemFactory.h>
#include <agrum/PRM/o3prm/O3TypeFactory.h>
#include <agrum/PRM/o3prm/cocoR/Parser.h>
#include <agrum/PRM/o3prm/cocoR/Scanner.h>
#include <agrum/agrum.h>

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
      template < typename GUM_SCALAR >
      class O3prmReader {
        public:
        O3prmReader();
        explicit O3prmReader(PRM< GUM_SCALAR >& prm);
        O3prmReader(const O3prmReader& src);
        O3prmReader(O3prmReader&& src);
        ~O3prmReader();
        O3prmReader& operator=(const O3prmReader& src);
        O3prmReader& operator=(O3prmReader&& src);

        /// Read file and load its content using a PRMFactory.
        /// The package parameter set the file's content package.
        Size readFile(const std::string& file, const std::string& module = "");

        /// With readString method, you must set the current path
        /// to search from import yourself, using addClassPath.
        Size readString(const std::string& string);

        void parseStream(std::istream& input, std::ostream& output, std::string module = "");
        /**
         * @brief This methods defines the list of paths to look for o3prm
         * files.
         *
         * Use / for path separator ! Even on Windows !
         *
         * @param class_path A semicolon separated list of paths.
         */
        void setClassPath(const std::string& class_path);

        /**
         * @brief Add a list of paths to look for o3prm
         * files.
         *
         * Use / for path separator ! Even on Windows !
         *
         * @param class_path A semicolon separated list of paths.
         */
        void addClassPath(const std::string& class_path);

        gum::prm::PRM< GUM_SCALAR >*       prm() { return _prm_; }
        const gum::prm::PRM< GUM_SCALAR >* prm() const { return _prm_; }

        /// @{
        /// publishing Errors API

        /// # of errors
        Size errors() const;
        /// # of errors
        Size warnings() const;

        ///
        const ErrorsContainer& errorsContainer() const;

        /// line of ith error or warning
        Idx errLine(Idx i) const;
        /// col of ith error or warning
        Idx errCol(Idx i) const;
        /// filename of ith error or warning
        std::wstring errFilename(Idx i) const;
        /// type of ith error or warning
        bool errIsError(Idx i) const;
        /// message of ith error or warning
        std::string errMsg(Idx i) const;

        /// send on std::cerr the list of errors
        void showElegantErrors(std::ostream& o = std::cerr) const;

        /// send on std::cerr the list of errors or warnings
        void showElegantErrorsAndWarnings(std::ostream& o = std::cerr) const;

        /// send on std::cerr the number of errors and the number of warnings
        void showErrorCounts(std::ostream& o = std::cerr) const;
        /// @}

        private:
        PRM< GUM_SCALAR >*         _prm_;
        std::unique_ptr< O3PRM >   _o3_prm_;
        std::vector< std::string > _class_path_;
        Set< std::string >         _imported_;

        // Needed when file can't be parse (can not open it for exemple)
        ErrorsContainer _errors_;

        void _readStream_(std::istream& input, const std::string& file, std::string module = "");

        void _parseImport_(const O3Import& i, const std::string& module_path);

        void _parseStream_(std::istream&      input,
                           const std::string& filename,
                           const std::string& module);

        std::vector< const O3Import* > _copyImports_();

        std::string _clean_(std::string text) const;
        std::string _print_(const ParseError& err) const;
        std::string _readStream_(std::istream& input);
      };

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum

// always include the implementation of the templates
#include <agrum/PRM/o3prm/O3prmReader_tpl.h>


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::prm::o3prm::O3prmReader< double >;
#endif


#endif   // GUM_PRM_O3PRM_O3PRM_FACTORY_H
