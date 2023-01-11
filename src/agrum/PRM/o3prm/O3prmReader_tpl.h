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
 * @brief Implementation for the O3prmReader class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */
#include <agrum/PRM/o3prm/O3prmReader.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE std::string O3prmReader< GUM_SCALAR >::_clean_(std::string text) const {
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
      INLINE std::string O3prmReader< GUM_SCALAR >::_print_(const ParseError& err) const {
        std::stringstream s;
        s << err.filename << "|" << err.line << " col " << err.column << "| " << _clean_(err.msg);
        return s.str();
      }

      template < typename GUM_SCALAR >
      INLINE std::string O3prmReader< GUM_SCALAR >::_readStream_(std::istream& inputstr) {
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
          _prm_(new PRM< GUM_SCALAR >()), _o3_prm_(std::unique_ptr< O3PRM >(new O3PRM())) {
        GUM_CONSTRUCTOR(O3prmReader);
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >::O3prmReader(PRM< GUM_SCALAR >& prm) :
          _prm_(&prm), _o3_prm_(std::unique_ptr< O3PRM >(new O3PRM())) {
        GUM_CONSTRUCTOR(O3prmReader);
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >::O3prmReader(const O3prmReader& src) :
          _prm_(src._prm_), _o3_prm_(std::unique_ptr< O3PRM >(new O3PRM(*(src._o3_prm_)))),
          _class_path_(src._class_path_), _imported_(src._imported_), _errors_(src._errors_) {
        GUM_CONS_CPY(O3prmReader);
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >::O3prmReader(O3prmReader&& src) :
          _prm_(std::move(src._prm_)), _o3_prm_(std::move(src._o3_prm_)),
          _class_path_(std::move(src._class_path_)), _imported_(std::move(src._imported_)),
          _errors_(std::move(src._errors_)) {
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
        _prm_        = src._prm_;
        _o3_prm_     = std::unique_ptr< O3PRM >(new O3PRM(*(src._o3_prm_)));
        _class_path_ = src._class_path_;
        _imported_   = src._imported_;
        _errors_     = src._errors_;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3prmReader< GUM_SCALAR >& O3prmReader< GUM_SCALAR >::operator=(O3prmReader&& src) {
        if (this == &src) { return *this; }
        _prm_        = std::move(src._prm_);
        _o3_prm_     = std::move(src._o3_prm_);
        _class_path_ = std::move(src._class_path_);
        _imported_   = std::move(src._imported_);
        _errors_     = std::move(src._errors_);
        return *this;
      }

      template < typename GUM_SCALAR >
      void O3prmReader< GUM_SCALAR >::setClassPath(const std::string& class_path) {
        _class_path_ = std::vector< std::string >();
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

        if (i < class_path.size()) { addClassPath(class_path.substr(i, std::string::npos)); }
      }

      template < typename GUM_SCALAR >
      void O3prmReader< GUM_SCALAR >::addClassPath(const std::string& class_path) {
        auto path = class_path;
        if (path[path.size() - 1] != '/') { path.append("/"); }
        auto dir = Directory(path);

        if (!dir.isValid()) {
          _errors_.addException("could not resolve class path", path);
        } else {
          _class_path_.push_back(std::move(path));
        }
      }

      template < typename GUM_SCALAR >
      INLINE Idx O3prmReader< GUM_SCALAR >::errLine(Idx i) const {
        return _errors_.error(i).line;
      }

      template < typename GUM_SCALAR >
      INLINE Idx O3prmReader< GUM_SCALAR >::errCol(Idx i) const {
        return _errors_.error(i).column;
      }

      template < typename GUM_SCALAR >
      INLINE std::wstring O3prmReader< GUM_SCALAR >::errFilename(Idx i) const {
        return widen(_errors_.error(i).filename);
      }

      template < typename GUM_SCALAR >
      INLINE bool O3prmReader< GUM_SCALAR >::errIsError(Idx i) const {
        return _errors_.error(i).is_error;
      }

      template < typename GUM_SCALAR >
      INLINE std::string O3prmReader< GUM_SCALAR >::errMsg(Idx i) const {
        return _errors_.error(i).msg;
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) const {
        for (Idx i = 0; i < _errors_.count(); ++i) {
          auto err = _errors_.error(i);
          if (err.is_error) { o << _print_(err) << std::endl; }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) const {
        for (Idx i = 0; i < _errors_.count(); ++i) {
          auto err = _errors_.error(i);
          o << _print_(err) << std::endl;
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) const {
        _errors_.syntheticResults(o);
      }

      template < typename GUM_SCALAR >
      INLINE Size O3prmReader< GUM_SCALAR >::errors() const {
        return _errors_.error_count;
      }

      template < typename GUM_SCALAR >
      INLINE Size O3prmReader< GUM_SCALAR >::warnings() const {
        return _errors_.warning_count;
      }

      template < typename GUM_SCALAR >
      INLINE const ErrorsContainer& O3prmReader< GUM_SCALAR >::errorsContainer() const {
        return _errors_;
      }

      template < typename GUM_SCALAR >
      INLINE Size O3prmReader< GUM_SCALAR >::readString(const std::string& str) {
        std::stringstream sBuff(str);
        _readStream_(sBuff, "");
        return _errors_.count();
      }

      template < typename GUM_SCALAR >
      INLINE Size O3prmReader< GUM_SCALAR >::readFile(const std::string& file,
                                                      const std::string& module) {
        try {
          auto lastSlashIndex = file.find_last_of('/');

          auto dir = Directory(file.substr(0, lastSlashIndex + 1));

          if (!dir.isValid()) {
            _errors_.addException("could not find file", file);
            return _errors_.count();
          }

          auto basename    = file.substr(lastSlashIndex + 1);
          auto absFilename = dir.absolutePath() + basename;

          std::ifstream input(absFilename);
          if (input.is_open()) {
            _readStream_(input, file, module);
          } else {
            _errors_.addException("could not open file", file);
          }

          return _errors_.count();

        } catch (gum::Exception& e) {
          GUM_SHOWERROR(e);
          _errors_.addException(e.errorContent(), file);
          return _errors_.count();
        } catch (...) {
          _errors_.addException("unknown error", file);
          return _errors_.count();
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::parseStream(std::istream& input,
                                                         std::ostream& output,
                                                         std::string   module) {
        _readStream_(input, "", module);

        showElegantErrorsAndWarnings(output);
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::_parseStream_(std::istream&      input,
                                                           const std::string& filename,
                                                           const std::string& module) {
        auto sBuff  = _readStream_(input);
        auto buffer = std::unique_ptr< unsigned char[] >(new unsigned char[sBuff.length() + 1]);
        strcpy((char*)buffer.get(), sBuff.c_str());
        auto s = o3prm_scanner(buffer.get(), int(sBuff.length() + 1), filename);
        auto p = o3prm_parser(&s);
        p.set_prm(_o3_prm_.get());
        p.set_prefix(module);
        p.Parse();
        _errors_ += p.errors();
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::_parseImport_(const O3Import&    i,
                                                           const std::string& module) {
        if (!_imported_.exists(i.import().label())) {
          _imported_.insert(i.import().label());

          auto module_path = module;
          std::replace(module_path.begin(), module_path.end(), '.', '/');

          auto path = i.import().label();
          std::replace(path.begin(), path.end(), '.', '/');

          auto imported = false;
          for (const auto& cp: _class_path_) {
            auto          file_path = cp + path + ".o3prm";
            std::ifstream file(file_path);

            if (file.is_open()) {
              _parseStream_(file, file_path, i.import().label());
              imported = true;
              break;
            }

            file_path = cp + module + path + ".o3prm";
            std::ifstream file2(file_path);

            if (file2.is_open()) {
              _parseStream_(file2, file_path, module + "." + i.import().label());
              imported = true;
              break;
            }
          }

          if (!imported) {
            const auto&       pos = i.import().position();
            std::stringstream msg;
            msg << "Import error: could not resolve import " << i.import().label();
            _errors_.addError(msg.str(), pos.file(), pos.line(), pos.column());
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE std::vector< const O3Import* > O3prmReader< GUM_SCALAR >::_copyImports_() {
        auto copy = std::vector< const O3Import* >();
        for (const auto& i: _o3_prm_->imports()) {
          if (!_imported_.exists(i->import().label())) { copy.push_back(i.get()); }
        }
        return copy;
      }

      template < typename GUM_SCALAR >
      INLINE void O3prmReader< GUM_SCALAR >::_readStream_(std::istream&      input,
                                                          const std::string& file,
                                                          std::string        module) {
        if (module.size() > 0 && module.back() != '.') { module.append("."); }

        _parseStream_(input, file, module);

        auto imports = _copyImports_();
        do {
          for (auto i: imports) {
            _parseImport_(*i, module);
          }
          imports = _copyImports_();
        } while (imports.size() > 0);


        if (_errors_.error_count == 0) {
          auto solver       = O3NameSolver< GUM_SCALAR >(*_prm_, *_o3_prm_, _errors_);
          auto type_factory = O3TypeFactory< GUM_SCALAR >(*_prm_, *_o3_prm_, solver, _errors_);

          auto interface_factory
             = O3InterfaceFactory< GUM_SCALAR >(*_prm_, *_o3_prm_, solver, _errors_);
          auto class_factory = O3ClassFactory< GUM_SCALAR >(*_prm_, *_o3_prm_, solver, _errors_);

          auto system_factory = O3SystemFactory< GUM_SCALAR >(*_prm_, *_o3_prm_, solver, _errors_);

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
          } catch (Exception const&) {
            if (_errors_.count() == 0) { _errors_.addException("an unknown error occured", file); }
          } catch (...) { _errors_.addException("an unknown exception occured", file); }
        }
      }
    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
