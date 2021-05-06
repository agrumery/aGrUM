/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementation for the O3prmReader class.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Lionel TORTI
 */
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE std::string
             O3prmReader< GUM_SCALAR >::clean__(std::string text) const {
        // This could be way more faster with regex but there are not implemented
        // with gcc-4.8 !
        text = replace(text, "Syntax error", "Error");

        text = replace(text, "LABEL_OR_STAR_LIST", "declaration");

        text = replace(text, "ARRAY_REFERENCE_SLOT", "declaration");
        text = replace(text, "FLOAT_AS_LABEL", "declaration");
        text = replace(text, "FLOAT_OR_INT", "declaration");
        text = replace(text, "INTEGER_AS_FLOAT", "declaration");
        text = replace(text, "INTEGER_AS_LABEL", "declaration");
        text = replace(text, "INT_TYPE_DECLARATION", "declaration");
        text = replace(text, "LABEL_OR_INT", "declaration");
        text = replace(text, "LABEL_OR_STAR", "declaration");
        text = replace(text, "NAMED_CLASS_ELEMENT", "declaration");
        text = replace(text, "REAL_TYPE_DECLARATION", "declaration");

        text = replace(text, "AGGREGATE_PARENTS", "declaration");
        text = replace(text, "CLASS_BODY", "declaration");
        text = replace(text, "CLASS_DECLARATION", "declaration");
        text = replace(text, "CLASS_ELEMENT", "declaration");
        text = replace(text, "CLASS_PARAMETER", "declaration");
        text = replace(text, "CLASS_UNIT", "declaration");
        text = replace(text, "FLOAT_LIST", "declaration");
        text = replace(text, "FORMULA_LIST", "declaration");
        text = replace(text, "IDENTIFIER_LIST", "declaration");
        text = replace(text, "IMPORT_BODY", "declaration");
        text = replace(text, "IMPORT_DECLARATION", "declaration");
        text = replace(text, "IMPORT_UNIT", "declaration");
        text = replace(text, "INTERFACE_BODY", "declaration");
        text = replace(text, "INTERFACE_DECLARATION", "declaration");
        text = replace(text, "INTERFACE_UNIT", "declaration");
        text = replace(text, "LABEL_LIST", "declaration");
        text = replace(text, "PARAMETER_LIST", "declaration");
        text = replace(text, "PREFIXED_LABEL", "declaration");
        text = replace(text, "RAW_CPT", "declaration");
        text = replace(text, "REFERENCE_SLOT", "declaration");
        text = replace(text, "RULE_CPT", "declaration");
        text = replace(text, "SYSTEM_BODY", "declaration");
        text = replace(text, "SYSTEM_DECLARATION", "declaration");
        text = replace(text, "SYSTEM_UNIT", "declaration");
        text = replace(text, "TYPE_DECLARATION", "declaration");
        text = replace(text, "TYPE_LABEL", "declaration");
        text = replace(text, "TYPE_UNIT", "declaration");
        text = replace(text, "TYPE_VALUE_LIST", "declaration");

        text = replace(text, "AGGREGATE", "declaration");
        text = replace(text, "ARRAY", "declaration");
        text = replace(text, "ATTRIBUTE", "declaration");
        text = replace(text, "CAST", "declaration");
        text = replace(text, "CHAIN", "declaration");
        text = replace(text, "CLASS", "declaration");
        text = replace(text, "FLOAT", "declaration");
        text = replace(text, "FORMULA", "declaration");
        text = replace(text, "IDENTIFIER", "declaration");
        text = replace(text, "INT", "declaration");
        text = replace(text, "INTEGER", "declaration");
        text = replace(text, "INTERFACE", "declaration");
        text = replace(text, "LABEL", "declaration");
        text = replace(text, "LINK", "declaration");
        text = replace(text, "MAP", "declaration");
        text = replace(text, "PARAMETER", "declaration");
        text = replace(text, "REAL", "declaration");
        text = replace(text, "RULE", "declaration");
        text = replace(text, "TYPE", "declaration");
        text = replace(text, "UNIT", "declaration");

        return text;
      }

      template < typename GUM_SCALAR >
      INLINE std::string
             O3prmReader< GUM_SCALAR >::print__(const ParseError& err) const {
        std::stringstream s;
        s << err.filename << "|" << err.line << " col " << err.column << "| "
          << clean__(err.msg);
        return s.str();
      }

      template < typename GUM_SCALAR >
      INLINE std::string
             O3prmReader< GUM_SCALAR >::readStream__(std::istream& inputstr) {
        if (inputstr) {
          inputstr.seekg(0, inputstr.end);
          int length = int(inputstr.tellg());
          inputstr.seekg(0, inputstr.beg);

          auto str = std::string();
          str.resize(length, ' ');
          auto begin = &*str.begin();

          inputstr.read(begin, length);

          return str;
        }
        GUM_ERROR(OperationNotAllowed, "Could not open file")
      }

      using o3prm_scanner = gum::prm::o3prm::Scanner;
      using o3prm_parser  = gum::prm::o3prm::Parser;

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >::O3prmReader() :
          prm__(new PRM< GUM_SCALAR >()),
          o3_prm__(std::unique_ptr< O3PRM >(new O3PRM())) {
        GUM_CONSTRUCTOR(O3prmReader);
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >::O3prmReader(PRM< GUM_SCALAR >& prm) :
          prm__(&prm), o3_prm__(std::unique_ptr< O3PRM >(new O3PRM())) {
        GUM_CONSTRUCTOR(O3prmReader);
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >::O3prmReader(const O3prmReader& src) :
          prm__(src.prm__),
          o3_prm__(std::unique_ptr< O3PRM >(new O3PRM(*(src.o3_prm__)))),
          class_path__(src.class_path__), imported__(src.imported__),
          errors__(src.errors__) {
        GUM_CONS_CPY(O3prmReader);
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >::O3prmReader(O3prmReader&& src) :
          prm__(std::move(src.prm__)), o3_prm__(std::move(src.o3_prm__)),
          class_path__(std::move(src.class_path__)),
          imported__(std::move(src.imported__)),
          errors__(std::move(src.errors__)) {
        GUM_CONS_CPY(O3prmReader);
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >::~O3prmReader() {
        GUM_DESTRUCTOR(O3prmReader);
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >&
         O3prmReader< GUM_SCALAR >::operator=(const O3prmReader& src) {
        if (this == &src) { return *this; }
        prm__        = src.prm__;
        o3_prm__     = std::unique_ptr< O3PRM >(new O3PRM(*(src.o3_prm__)));
        class_path__ = src.class_path__;
        imported__   = src.imported__;
        errors__     = src.errors__;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >&
         O3prmReader< GUM_SCALAR >::operator=(O3prmReader&& src) {
        if (this == &src) { return *this; }
        prm__        = std::move(src.prm__);
        o3_prm__     = std::move(src.o3_prm__);
        class_path__ = std::move(src.class_path__);
        imported__   = std::move(src.imported__);
        errors__     = std::move(src.errors__);
        return *this;
      }

      template < typename GUM_SCALAR >
      void O3prmReader< GUM_SCALAR >::setClassPath(const std::string& class_path) {
        class_path__ = std::vector< std::string >();
        size_t i     = 0;
        size_t j     = class_path.find(';');

        while (j != std::string::npos) {
          addClassPath(class_path.substr(i, j - i));
          i = j + 1;

          if (i < class_path.size()) {
            j = class_path.find(';', i);
          } else {
            j = std::string::npos;
          }
        }

        if (i < class_path.size()) {
          addClassPath(class_path.substr(i, std::string::npos));
        }
      }

      template < typename GUM_SCALAR >
      void O3prmReader< GUM_SCALAR >::addClassPath(const std::string& class_path) {
        auto path = class_path;
        if (path[path.size() - 1] != '/') { path.append("/"); }
        auto dir = Directory(path);

        if (!dir.isValid()) {
          errors__.addException("could not resolve class path", path);
        } else {
          class_path__.push_back(std::move(path));
        }
      }

      template < typename GUM_SCALAR >
      INLINE Idx O3prmReader< GUM_SCALAR >::errLine(Idx i) const {
        return errors__.error(i).line;
      }

      template < typename GUM_SCALAR >
      INLINE Idx O3prmReader< GUM_SCALAR >::errCol(Idx i) const {
        return errors__.error(i).column;
      }

      template < typename GUM_SCALAR >
      INLINE std::wstring O3prmReader< GUM_SCALAR >::errFilename(Idx i) const {
        return widen(errors__.error(i).filename);
      }

      template < typename GUM_SCALAR >
      INLINE bool O3prmReader< GUM_SCALAR >::errIsError(Idx i) const {
        return errors__.error(i).is_error;
      }

      template < typename GUM_SCALAR >
      INLINE std::string O3prmReader< GUM_SCALAR >::errMsg(Idx i) const {
        return errors__.error(i).msg;
      }

      template < typename GUM_SCALAR >
      INLINE void
         O3prmReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) const {
        for (Idx i = 0; i < errors__.count(); ++i) {
          auto err = errors__.error(i);
          if (err.is_error) { o << print__(err) << std::endl; }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::showElegantErrorsAndWarnings(
         std::ostream& o) const {
        for (Idx i = 0; i < errors__.count(); ++i) {
          auto err = errors__.error(i);
          o << print__(err) << std::endl;
        }
      }

      template < typename GUM_SCALAR >
      INLINE void
         O3prmReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) const {
        errors__.syntheticResults(o);
      }

      template < typename GUM_SCALAR >
      INLINE Size O3prmReader< GUM_SCALAR >::errors() const {
        return errors__.error_count;
      }

      template < typename GUM_SCALAR >
      INLINE Size O3prmReader< GUM_SCALAR >::warnings() const {
        return errors__.warning_count;
      }

      template < typename GUM_SCALAR >
      INLINE const ErrorsContainer&
                   O3prmReader< GUM_SCALAR >::errorsContainer() const {
        return errors__;
      }

      template < typename GUM_SCALAR >
      INLINE Size O3prmReader< GUM_SCALAR >::readString(const std::string& str) {
        std::stringstream sBuff(str);
        readStream__(sBuff, "");
        return errors__.count();
      }

      template < typename GUM_SCALAR >
      INLINE Size O3prmReader< GUM_SCALAR >::readFile(const std::string& file,
                                                      const std::string& module) {
        try {
          auto lastSlashIndex = file.find_last_of('/');

          auto dir = Directory(file.substr(0, lastSlashIndex + 1));

          if (!dir.isValid()) {
            errors__.addException("could not find file", file);
            return errors__.count();
          }

          auto basename    = file.substr(lastSlashIndex + 1);
          auto absFilename = dir.absolutePath() + basename;

          std::ifstream input(absFilename);
          if (input.is_open()) {
            readStream__(input, file, module);
          } else {
            errors__.addException("could not open file", file);
          }

          return errors__.count();

        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          errors__.addException(e.errorContent(), file);
          return errors__.count();
        } catch (...) {
          errors__.addException("unknown error", file);
          return errors__.count();
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::parseStream(std::istream& input,
                                                         std::ostream& output,
                                                         std::string   module) {
        readStream__(input, "", module);

        showElegantErrorsAndWarnings(output);
      }

      template < typename GUM_SCALAR >
      INLINE void
         O3prmReader< GUM_SCALAR >::parseStream__(std::istream&      input,
                                                  const std::string& filename,
                                                  const std::string& module) {
        auto sBuff  = readStream__(input);
        auto buffer = std::unique_ptr< unsigned char[] >(
           new unsigned char[sBuff.length() + 1]);
        strcpy((char*)buffer.get(), sBuff.c_str());
        auto s = o3prm_scanner(buffer.get(), int(sBuff.length() + 1), filename);
        auto p = o3prm_parser(&s);
        p.set_prm(o3_prm__.get());
        p.set_prefix(module);
        p.Parse();
        errors__ += p.errors();
      }

      template < typename GUM_SCALAR >
      INLINE void
         O3prmReader< GUM_SCALAR >::parseImport__(const O3Import&    i,
                                                  const std::string& module) {
        if (!imported__.exists(i.import().label())) {
          imported__.insert(i.import().label());

          auto module_path = module;
          std::replace(module_path.begin(), module_path.end(), '.', '/');

          auto path = i.import().label();
          std::replace(path.begin(), path.end(), '.', '/');

          auto imported = false;
          for (const auto& cp: class_path__) {
            auto          file_path = cp + path + ".o3prm";
            std::ifstream file(file_path);

            if (file.is_open()) {
              parseStream__(file, file_path, i.import().label());
              imported = true;
              break;
            }

            file_path = cp + module + path + ".o3prm";
            std::ifstream file2(file_path);

            if (file2.is_open()) {
              parseStream__(file2, file_path, module + "." + i.import().label());
              imported = true;
              break;
            }
          }

          if (!imported) {
            const auto&       pos = i.import().position();
            std::stringstream msg;
            msg << "Import error: could not resolve import " << i.import().label();
            errors__.addError(msg.str(), pos.file(), pos.line(), pos.column());
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE std::vector< const O3Import* >
             O3prmReader< GUM_SCALAR >::copyImports__() {
        auto copy = std::vector< const O3Import* >();
        for (const auto& i: o3_prm__->imports()) {
          if (!imported__.exists(i->import().label())) { copy.push_back(i.get()); }
        }
        return copy;
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::readStream__(std::istream&      input,
                                                          const std::string& file,
                                                          std::string module) {
        if (module.size() > 0 && module.back() != '.') { module.append("."); }

        parseStream__(input, file, module);

        auto imports = copyImports__();
        do {
          for (auto i: imports) {
            parseImport__(*i, module);
          }
          imports = copyImports__();
        } while (imports.size() > 0);


        if (errors__.error_count == 0) {
          auto solver = O3NameSolver< GUM_SCALAR >(*prm__, *o3_prm__, errors__);
          auto type_factory
             = O3TypeFactory< GUM_SCALAR >(*prm__, *o3_prm__, solver, errors__);

          auto interface_factory = O3InterfaceFactory< GUM_SCALAR >(*prm__,
                                                                    *o3_prm__,
                                                                    solver,
                                                                    errors__);
          auto class_factory
             = O3ClassFactory< GUM_SCALAR >(*prm__, *o3_prm__, solver, errors__);

          auto system_factory
             = O3SystemFactory< GUM_SCALAR >(*prm__, *o3_prm__, solver, errors__);

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
          } catch (Exception&) {
            if (errors__.count() == 0) {
              errors__.addException("an unknown error occured", file);
            }
            // GUM_TRACE_NEWLINE;
            // GUM_SHOWERROR( e );
          } catch (...) {
            errors__.addException("an unknown exception occured", file);
          }
        }
      }
    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
