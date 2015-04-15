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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <string>

// to ease automatic parsing
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum {

  namespace prm {

    namespace o3prm {

      template <typename GUM_SCALAR>
      void O3prmReader<GUM_SCALAR>::setClassPath(const std::string &class_path) {
        size_t i = 0;
        size_t j = class_path.find(';');

        while (j != std::string::npos) {
          addClassPath(class_path.substr(i, j - i));
          i = j + 1;

          if (i < class_path.size())
            j = class_path.find(';', i);
          else
            j = std::string::npos;
        }

        if (i < class_path.size())
          addClassPath(class_path.substr(i, std::string::npos));
      }

      template <typename GUM_SCALAR>
      void O3prmReader<GUM_SCALAR>::addClassPath(const std::string &class_path) {
        if (class_path[class_path.size() - 1] == '/')
          __class_path.push_back(class_path);
        else
          __class_path.push_back(class_path + '/');
      }

      template <typename GUM_SCALAR> INLINE O3prmReader<GUM_SCALAR>::O3prmReader() {
        GUM_CONSTRUCTOR(O3prmReader);
        __parseDone = false;
        __prmTake = false;
        __parser = nullptr;
      }

      template <typename GUM_SCALAR> INLINE O3prmReader<GUM_SCALAR>::~O3prmReader() {
        GUM_DESTRUCTOR(O3prmReader);

        if (__parseDone)
          if (__parser != nullptr)
            delete __parser;

        if (!__prmTake)
          delete __factory.prm();
      }

      template <typename GUM_SCALAR>
      INLINE int O3prmReader<GUM_SCALAR>::readFile(const std::string &file) {
        size_t lastSlashIndex = file.find_last_of('/');
        Directory dir(file.substr(0, lastSlashIndex + 1));

        if (!dir.isValid()) {
          __errors.addException("directory doesn't exist.", dir.path());
          return __errors.count();
        }

        std::string basename = file.substr(lastSlashIndex + 1);

        std::string absFilename = dir.absolutePath() + basename;

        try {
          if (__parser && __parser->getImports().exists(absFilename))
            return __errors.count();
          ;

          Scanner s(absFilename.c_str());

          if (!__parseDone) {
            __parser = new Parser(&s);
            __parser->setFactory(&__factory);
            __parser->setClassPath(__class_path);
          } else
            __parser->scanner = &s;

          __parser->setCurrentDirectory(dir.absolutePath());

          __parser->addImport(absFilename);

          __parser->Parse();

          __parseDone = true;

          __errors += __parser->errors();
        } catch (gum::Exception &e) {
          GUM_SHOWERROR(e);
          __errors.addException(e.errorContent(), file);
        }

        return __parser->errors().count();
      }

      template <typename GUM_SCALAR>
      INLINE int O3prmReader<GUM_SCALAR>::readString(const std::string &st) {
        // errors += parser.errors
        try {
          Scanner s((unsigned char *)st.c_str(), (int)(st.size()));
          __parser = new Parser(&s);
          __parser->setFactory(&__factory);
          __parser->setClassPath(__class_path);

          __parser->Parse();
          __parseDone = true;
          __errors += __parser->errors();
        } catch (gum::Exception &e) {
          GUM_SHOWERROR(e);
          __errors.addException(e.errorContent(), "");
        }

        return __parser->errors().count();
      }

      template <typename GUM_SCALAR>
      INLINE gum::prm::PRM<GUM_SCALAR> *O3prmReader<GUM_SCALAR>::prm() {
        __prmTake = true;
        return __factory.prm();
      }

      template <typename GUM_SCALAR>
      INLINE const gum::prm::PRM<GUM_SCALAR> *O3prmReader<GUM_SCALAR>::prm() const {
        return __factory.prm();
      }

      /// publishing Errors API
      template <typename GUM_SCALAR>
      INLINE unsigned int O3prmReader<GUM_SCALAR>::errLine(unsigned int i) const {
        return __errors.line(i);
      }

      template <typename GUM_SCALAR>
      INLINE unsigned int O3prmReader<GUM_SCALAR>::errCol(unsigned int i) const {
        return __errors.col(i);
      }

      template <typename GUM_SCALAR>
      INLINE std::wstring
      O3prmReader<GUM_SCALAR>::errFilename(unsigned int i) const {
        return __errors.filename(i);
      }

      template <typename GUM_SCALAR>
      INLINE bool O3prmReader<GUM_SCALAR>::errIsError(unsigned int i) const {
        return __errors.is_error(i);
      }

      template <typename GUM_SCALAR>
      INLINE std::string O3prmReader<GUM_SCALAR>::errMsg(unsigned int i) const {
        return gum::narrow(__errors.msg(i));
      }

      template <typename GUM_SCALAR>
      INLINE void O3prmReader<GUM_SCALAR>::showElegantErrors(std::ostream &o) const {
        __errors.elegantErrors(o);
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3prmReader<GUM_SCALAR>::showElegantErrorsAndWarnings(std::ostream &o) const {
        __errors.elegantErrorsAndWarnings(o);
      }

      template <typename GUM_SCALAR>
      INLINE void O3prmReader<GUM_SCALAR>::showErrorCounts(std::ostream &o) const {
        __errors.syntheticResults(o);
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
      INLINE const ErrorsContainer &
      O3prmReader<GUM_SCALAR>::errorsContainer() const {
        return __errors;
      }

    } /* namespace o3prm */
  }   /* namespace prm */
} /* namespace gum */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
