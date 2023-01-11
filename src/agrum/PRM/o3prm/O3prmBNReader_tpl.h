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
 * @brief Inline implementation of O3prmReader.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Lionel TORTI
 */

// to ease Parser
#include <agrum/PRM/o3prm/O3prmBNReader.h>

namespace gum {
  template < typename GUM_SCALAR >
  INLINE std::string O3prmBNReader< GUM_SCALAR >::_getVariableName_(const std::string& path,
                                                                    const std::string& type,
                                                                    const std::string& name,
                                                                    const std::string& toRemove) {
    auto res = path + name;   // path ends up with a "."
    if (toRemove != "") {
      if (res.substr(0, toRemove.size()) == toRemove) { res = res.substr(toRemove.size()); }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  std::string O3prmBNReader< GUM_SCALAR >::_getInstanceName_(const std::string& classname) {
    auto res = classname.substr(0, 4);
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
  }

  template < typename GUM_SCALAR >
  std::string O3prmBNReader< GUM_SCALAR >::_getEntityName_(const std::string& filename) {
    auto b = filename.find_last_of("/\\");
    auto e = filename.find_last_of(".") - 1;
    GUM_ASSERT(e > b);   // we are waiting ../../basename.o3prm
    return filename.substr(b + 1, e - b);
  }

  template < typename GUM_SCALAR >
  O3prmBNReader< GUM_SCALAR >::O3prmBNReader(BayesNet< GUM_SCALAR >* bn,
                                             const std::string&      filename,
                                             const std::string&      entityName,
                                             const std::string&      classpath) :
      BNReader< GUM_SCALAR >(bn, filename) {
    GUM_CONSTRUCTOR(O3prmBNReader);
    _bn_         = bn;
    _filename_   = filename;
    _entityName_ = entityName.empty() ? _getEntityName_(filename) : entityName;
    _classpath_  = classpath;
  }

  template < typename GUM_SCALAR >
  O3prmBNReader< GUM_SCALAR >::~O3prmBNReader() {
    GUM_DESTRUCTOR(O3prmBNReader);
  }

  /// parse.
  /// @return the number of detected errors
  /// @throws IOError if file not exists
  template < typename GUM_SCALAR >
  Size O3prmBNReader< GUM_SCALAR >::proceed() {
    prm::o3prm::O3prmReader< GUM_SCALAR > reader;
    if (_classpath_ != "") { reader.addClassPath(_classpath_); }
    reader.readFile(_filename_);
    gum::prm::PRM< GUM_SCALAR >* prm = reader.prm();
    _errors_                         = reader.errorsContainer();


    if (errors() == 0) {
      std::string instanceName = "";
      if (prm->isSystem(_entityName_)) {
        _generateBN_(prm->getSystem(_entityName_));
      } else if (prm->isClass(_entityName_)) {
        ParseError warn(false,
                        "No system '" + _entityName_
                           + "' found but class found. Generating unnamed instance.",
                        _filename_,
                        0);
        _errors_.add(warn);
        gum::prm::PRMSystem< GUM_SCALAR > s("S_" + _entityName_);
        instanceName = _getInstanceName_(_entityName_);
        auto i = new gum::prm::PRMInstance< GUM_SCALAR >(instanceName, prm->getClass(_entityName_));
        s.add(i);
        _generateBN_(s);
        instanceName += ".";   // to be removed in  _getVariableName_
      } else if (prm->classes().size() == 1) {
        const std::string& entityName = (*prm->classes().begin())->name();
        ParseError         warn(false,
                        "Unique class '" + entityName + "' found. Generating unnamed instance.",
                        _filename_,
                        0);
        _errors_.add(warn);

        gum::prm::PRMSystem< GUM_SCALAR > s("S_" + entityName);
        instanceName = _getInstanceName_(entityName);
        auto i = new gum::prm::PRMInstance< GUM_SCALAR >(instanceName, prm->getClass(entityName));
        s.add(i);
        _generateBN_(s);

        // force the name of the BN to be the name of the class instead of the name
        // of the file
        _bn_->setProperty("name", entityName);
        instanceName += ".";   // to be removed in  _getVariableName_
      } else {
        ParseError err(true,
                       "Neither system nor class '" + _entityName_ + "' and more than one class.",
                       _filename_,
                       0);
        _errors_.add(err);
      }

      // renaming variables in th BN
      gum::Set< std::string > names;
      for (auto node: _bn_->nodes()) {
        // keeping the complete name in description
        const std::string& nn = _bn_->variable(node).name();
        _bn_->variable(node).setDescription(nn);

        // trying to simplify the
        auto start = nn.find_first_of('(');
        auto end   = nn.find_first_of(')');
        if (0 < start && start < end && end < nn.size()) {
          auto path = nn.substr(0, start);
          auto type = nn.substr(start + 1, end - start - 1);
          auto name = nn.substr(end + 1, std::string::npos);

          std::string newNameRadical = _getVariableName_(path, type, name, instanceName);

          std::string newName = newNameRadical;
          // forcing newName to be unique
          int num = 0;
          while (names.contains(newName)) {
            newName = newNameRadical + std::to_string(++num);
          }

          names.insert(newName);
          _bn_->changeVariableName(node, newName);
        } else {
          ParseError warn(false, "Name " + nn + " cannot be simplified.", _filename_, 0);
          _errors_.add(warn);
        }
      }
    }

    delete prm;

    return errors();
  }


  template < typename GUM_SCALAR >
  void O3prmBNReader< GUM_SCALAR >::_generateBN_(prm::PRMSystem< GUM_SCALAR >& system) {
    system.instantiate();
    BayesNetFactory< GUM_SCALAR > factory(_bn_);
    system.groundedBN(factory);
    _bn_->setProperty("name", _entityName_);
  }
}   // namespace gum
