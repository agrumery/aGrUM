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
 * @brief Headers of O3prmReader.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN & PH WUILLEMIN
 */

#ifndef GUM_SKOOL_READER_H
#define GUM_SKOOL_READER_H

#include <fstream>

#include <agrum/PRM/PRMFactory.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// including coco-generated PARSER and SCANNER

#include <agrum/PRM/o3prm/cocoR/Parser.h>
#endif // DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  namespace prm {
    namespace o3prm {
      template <typename GUM_SCALAR> class O3prmReader {
        public:
        O3prmReader();

        ~O3prmReader();

        int readFile(const std::string &file);
        /// With readString method, you must set the current path
        /// to search from import yourself, using addClassPath.
        int readString(const std::string &string);

        /**
         * @brief This methods defines the list of paths to look for o3prm files.
         * Use / for path separator ! Even on Windows !
         *
         * @param class_path A semicolon separated list of paths.
         */
        void setClassPath(const std::string &class_path);
        void addClassPath(const std::string &class_path);

        gum::prm::PRM<GUM_SCALAR> *prm();
        const gum::prm::PRM<GUM_SCALAR> *prm() const;

        /// @{
        /// publishing Errors API

        /// # of errors
        Size errors() const;
        /// # of errors
        Size warnings() const;

        ///
        const ErrorsContainer &errorsContainer() const;

        /// line of ith error or warning
        unsigned int errLine(unsigned int i) const;
        /// col of ith error or warning
        unsigned int errCol(unsigned int i) const;
        /// filename of ith error or warning
        std::wstring errFilename(unsigned int i) const;
        /// type of ith error or warning
        bool errIsError(unsigned int i) const;
        /// message of ith error or warning
        std::string errMsg(unsigned int i) const;

        /// send on std::cerr the list of errors
        void showElegantErrors(std::ostream &o = std::cerr) const;

        /// send on std::cerr the list of errors or warnings
        void showElegantErrorsAndWarnings(std::ostream &o = std::cerr) const;

        /// send on std::cerr the number of errors and the number of warnings
        void showErrorCounts(std::ostream &o = std::cerr) const;
        /// @}

        private:
        O3prmReader(const O3prmReader &source);

        O3prmReader &operator=(const O3prmReader &source);

        gum::prm::PRMFactory<GUM_SCALAR> __factory;

        std::vector<std::string> __class_path;

        Parser *__parser;
        bool __parseDone;
        bool __prmTake;
        // Needed when file can't be parse (can not open it for exemple)
        ErrorsContainer __errors;
      };

      extern template class O3prmReader<double>;
    } /* namespace o3prm */
  }   /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/o3prm/O3prmReader.tcc>

#endif /* GUM_SKOOL_READER_H */
