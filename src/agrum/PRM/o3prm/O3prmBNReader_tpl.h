
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Lionel TORTI
 */

// to ease Parser
#include <agrum/PRM/o3prm/O3prmBNReader.h>

namespace gum {
  template < typename GUM_SCALAR >
  INLINE std::string
         O3prmBNReader< GUM_SCALAR >::getVariableName__(const std::string& path,
                                                    const std::string& type,
                                                    const std::string& name,
                                                    const std::string& toRemove) {
    auto res = path + name;   // path ends up with a "."
    if (toRemove != "") {
      if (res.substr(0, toRemove.size()) == toRemove) {
        res = res.substr(toRemove.size());
      }
    }
    return res;
  }

  template < typename GUM_SCALAR >
  std::string
     O3prmBNReader< GUM_SCALAR >::getInstanceName__(const std::string& classname) {
    auto res = classname.substr(0, 4);
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
  }

  template < typename GUM_SCALAR >
  std::string
     O3prmBNReader< GUM_SCALAR >::getEntityName__(const std::string& filename) {
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
    bn__ = bn;
    filename__ = filename;
    entityName__ = entityName == "" ? getEntityName__(filename) : entityName;
    classpath__ = classpath;
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
    if (classpath__ != "") { reader.addClassPath(classpath__); }
    reader.readFile(filename__);
    gum::prm::PRM< GUM_SCALAR >* prm = reader.prm();
    errors__ = reader.errorsContainer();


    if (errors() == 0) {
      std::string instanceName = "";
      if (prm->isSystem(entityName__)) {
        generateBN__(prm->getSystem(entityName__));
      } else if (prm->isClass(entityName__)) {
        ParseError warn(
           false,
           "No system '" + entityName__
              + "' found but class found. Generating unnamed instance.",
           filename__,
           0);
        errors__.add(warn);
        gum::prm::PRMSystem< GUM_SCALAR > s("S_" + entityName__);
        instanceName = getInstanceName__(entityName__);
        auto i = new gum::prm::PRMInstance< GUM_SCALAR >(
           instanceName, prm->getClass(entityName__));
        s.add(i);
        generateBN__(s);
        instanceName += ".";   // to be removed in getVariableName__
      } else if (prm->classes().size() == 1) {
        const std::string& entityName = (*prm->classes().begin())->name();
        ParseError         warn(false,
                        "Unique class '" + entityName
                           + "' found. Generating unnamed instance.",
                        filename__,
                        0);
        errors__.add(warn);

        gum::prm::PRMSystem< GUM_SCALAR > s("S_" + entityName);
        instanceName = getInstanceName__(entityName);
        auto i = new gum::prm::PRMInstance< GUM_SCALAR >(
           instanceName, prm->getClass(entityName));
        s.add(i);
        generateBN__(s);

        // force the name of the BN to be the name of the class instead of the name
        // of the file
        bn__->setProperty("name", entityName);
        instanceName += ".";   // to be removed in getVariableName__
      } else {
        ParseError err(true,
                       "Neither system nor class '" + entityName__
                          + "' and more than one class.",
                       filename__,
                       0);
        errors__.add(err);
      }

      // renaming variables in th BN
      gum::Set< std::string > names;
      for (auto node: bn__->nodes()) {
        // keeping the complete name in description
        const std::string& nn = bn__->variable(node).name();
        bn__->variable(node).setDescription(nn);

        // trying to simplify the
        auto start = nn.find_first_of('(');
        auto end = nn.find_first_of(')');
        if (0 < start && start < end && end < nn.size()) {
          auto path = nn.substr(0, start);
          auto type = nn.substr(start + 1, end - start - 1);
          auto name = nn.substr(end + 1, std::string::npos);

          std::string newNameRadical =
             getVariableName__(path, type, name, instanceName);

          std::string newName = newNameRadical;
          // forcing newName to be unique
          int num = 0;
          while (names.contains(newName)) {
            newName = newNameRadical + std::to_string(++num);
          }

          names.insert(newName);
          bn__->changeVariableName(node, newName);
        } else {
          ParseError warn(
             false, "Name " + nn + " cannot be simplified.", filename__, 0);
          errors__.add(warn);
        }
      }
    }

    delete prm;

    return errors();
  }


  template < typename GUM_SCALAR >
  void O3prmBNReader< GUM_SCALAR >::generateBN__(
     prm::PRMSystem< GUM_SCALAR >& system) {
    system.instantiate();
    BayesNetFactory< GUM_SCALAR > factory(bn__);
    system.groundedBN(factory);
    bn__->setProperty("name", entityName__);
  }
}   // namespace gum
