/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

/*
 * gum::Configuration is wrapped by hand (its header is not %include-d): its nested types,
 * its hooks (std::function) and its mutexes have no meaning in a target language, which only
 * needs the plain-string subset below. Hooks of the target language are handled by the
 * language layer (see pyLibs/config.py for pyAgrum).
 */

%{
#include <agrum/base/core/configuration/configuration.h>

// default of the argument allowModificationWhenSaving of the save* methods: the configuration
inline bool gumConfigAllowModificationWhenSaving() {
  return gum::Configuration::instance().getTyped< bool >("core", "allow_modification_when_saving");
}

namespace {
  std::string _gumConfigurationKindName(gum::Configuration::Kind kind) {
    switch (kind) {
      case gum::Configuration::Kind::INT       : return "int";
      case gum::Configuration::Kind::FLOAT     : return "float";
      case gum::Configuration::Kind::BOOL      : return "bool";
      case gum::Configuration::Kind::ENUM      : return "enum";
      case gum::Configuration::Kind::DEPRECATED: return "deprecated";
      default                                  : return "str";
    }
  }
}   // namespace
%}

%rename(_Configuration) gum::Configuration;

namespace gum {
  class Configuration {
    public:
    explicit Configuration(std::string_view defaultsText, std::string_view origin = "defaults.ini");

    static gum::Configuration& instance();
    static std::string         userFileName();

    std::string get(std::string_view section, std::string_view key) const;
    bool        has(std::string_view section, std::string_view key) const;
    std::string doc(std::string_view section, std::string_view key) const;
    std::vector< std::string > sections() const;

    void set(std::string_view section,
             std::string_view key,
             std::string_view value,
             bool             noHook = false);
    void reset();
    void push();
    void pop();

    void load(const std::string& filename);
    void save(const std::string& filename) const;

    std::string grep(std::string_view search) const;
    std::string diff() const;
    std::string toString() const;

    void runHooks() const;
  };
}   // namespace gum

%extend gum::Configuration {
  // declared type of section.key, following a deprecation to the key that carries the value:
  // "int", "float", "bool", "str" or "enum"
  std::string resolvedKind(std::string_view section, std::string_view key) const {
    auto meta = self->meta(section, key);
    if (meta.kind == gum::Configuration::Kind::DEPRECATED) {
      meta = self->meta(meta.targetSection, meta.targetKey);
    }
    return _gumConfigurationKindName(meta.kind);
  }

  // the whole schema, flattened: 8 strings per key, in the order of the defaults
  //   section, key, kind (int|float|bool|str|enum|deprecated), allowed values joined by '|' (enum),
  //   documentation, replacement section and key (deprecated), default value
  std::vector< std::string > schemaRows() const {
    std::vector< std::string > rows;
    for (const auto& entry: self->schema()) {
      std::string values;
      for (const auto& v: entry.meta.values) {
        if (!values.empty()) values += '|';
        values += v;
      }
      rows.push_back(entry.section);
      rows.push_back(entry.key);
      rows.push_back(_gumConfigurationKindName(entry.meta.kind));
      rows.push_back(values);
      rows.push_back(entry.meta.doc);
      rows.push_back(entry.meta.targetSection);
      rows.push_back(entry.meta.targetKey);
      rows.push_back(entry.defaultValue);
    }
    return rows;
  }

  // the queued notices, flattened: 2 strings per notice ("deprecation" or "file", message)
  std::vector< std::string > takeNoticeRows() {
    std::vector< std::string > rows;
    for (const auto& notice: self->takeNotices()) {
      rows.push_back(notice.kind == gum::Configuration::Notice::Kind::DEPRECATION ? "deprecation"
                                                                                   : "file");
      rows.push_back(notice.message);
    }
    return rows;
  }
}
