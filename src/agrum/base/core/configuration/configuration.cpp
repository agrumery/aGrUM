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


/**
 * @file
 * @brief Implementation of gum::Configuration.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to ease automatic parsers
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <agrum/agrum.h>

#include <agrum/base/core/configuration/configuration.h>
#ifdef GUM_NO_INLINE
#  include <agrum/base/core/configuration/configuration_inl.h>
#endif   // GUM_NO_INLINE
#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/threads/threads.h>

#include <agrum/base/core/utils_string.h>

// the whole of aGrUM is compiled with GUM_FOR_SWIG for a pyAgrum build: the embedded defaults are
// then those of aGrUM followed by those of pyAgrum (two sources, embedded by cmake), and the user
// file is pyagrum.ini. Both files only change existing keys: they never create a section or a key.
#ifdef GUM_FOR_SWIG
#  define GUM_CONFIGURATION_USER_FILE "pyagrum.ini"
#else
#  define GUM_CONFIGURATION_USER_FILE "agrum.ini"
#endif

namespace gum {

  // the content of the defaults.ini of aGrUM and of the one of pyAgrum (empty if pyAgrum is not
  // compiled), generated at build time (see src/cmake)
  std::string_view configurationEmbeddedDefaults();
  std::string_view configurationEmbeddedPyAgrumDefaults();

  namespace {

    constexpr std::string_view _undocumented_ = "(undocumented)";

    // the keys of section core copied in atomics (see Configuration::epsilonCoarse() and so on), in
    // the order of Configuration::_epsilons_
    constexpr std::array< std::string_view, 3 > _epsilonKeys_
        = {"epsiloncoarse", "epsilonstandard", "epsilonfine"};

    [[noreturn]] void
        _syntaxError_(const std::string& msg, std::string_view origin, std::size_t line) {
      GUM_SYNTAX_ERROR(msg, std::string(origin), line, 0)
    }

    bool _isCommentOrBlank_(std::string_view trimmed) {
      return trimmed.empty() || trimmed.front() == '#' || trimmed.front() == ';';
    }

    // `###` followed by anything but another `#`
    bool _isMetaLine_(std::string_view trimmed) {
      return trimmed.size() >= 3 && trimmed.substr(0, 3) == "###"
          && (trimmed.size() == 3 || trimmed[3] != '#');
    }

    bool _isSectionLine_(std::string_view trimmed, std::string& name) {
      if (trimmed.size() < 3 || trimmed.front() != '[' || trimmed.back() != ']') return false;
      const auto inner = trimmed.substr(1, trimmed.size() - 2);
      if (inner.find(']') != std::string_view::npos) return false;
      name = std::string(inner);
      return true;
    }

    // `key = value` or `key : value`, whichever delimiter comes first
    bool _splitKeyValue_(std::string_view trimmed, std::string& key, std::string& value) {
      const auto pos = trimmed.find_first_of("=:");
      if (pos == std::string_view::npos) return false;
      key   = trim_copy(trimmed.substr(0, pos));
      value = trim_copy(trimmed.substr(pos + 1));
      return !key.empty();
    }

    bool _isIdentifier_(std::string_view s) {
      if (s.empty() || !(std::isalpha(static_cast< unsigned char >(s[0])) || s[0] == '_')) {
        return false;
      }
      return std::all_of(s.begin(), s.end(), [](char c) {
        return std::isalnum(static_cast< unsigned char >(c)) || c == '_';
      });
    }

    bool _hasSpace_(std::string_view s) {
      return std::any_of(s.begin(), s.end(), [](char c) {
        return std::isspace(static_cast< unsigned char >(c));
      });
    }

    std::vector< std::string > _lines_(std::string_view text) {
      std::vector< std::string > lines;
      std::size_t                prev = 0;
      while (prev <= text.size()) {
        auto pos = text.find('\n', prev);
        if (pos == std::string_view::npos) pos = text.size();
        auto line = text.substr(prev, pos - prev);
        if (!line.empty() && line.back() == '\r') line.remove_suffix(1);
        lines.emplace_back(line);
        prev = pos + 1;
      }
      if (!lines.empty() && lines.back().empty()) lines.pop_back();   // text ending with '\n'
      return lines;
    }

    bool _toInt_(std::string_view s, std::int64_t& res) {
      if (!s.empty() && s.front() == '+') s.remove_prefix(1);
      if (s.empty()) return false;
      const auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), res);
      return ec == std::errc() && ptr == s.data() + s.size();
    }

    // parses the domain of an int or float (e.g. `]0,1[`, `[1,]`); on failure, gives the reason in
    // error
    bool _parseDomain_(std::string_view             text,
                       bool                         isInt,
                       Configuration::Meta::Domain& domain,
                       std::string&                 error) {
      std::string t;
      for (const char c: text)
        if (!std::isspace(static_cast< unsigned char >(c))) t += c;

      const auto isBracket = [](char c) { return c == '[' || c == ']'; };
      const auto comma     = t.find(',');
      if (t.size() < 3 || !isBracket(t.front()) || !isBracket(t.back())
          || comma == std::string::npos || t.find(',', comma + 1) != std::string::npos) {
        error = "'" + t + "' is not a valid domain (e.g. ]0,1[ or [1,])";
        return false;
      }

      const auto parseBound = [&](const std::string& s, bool& has, double& value) {
        has = !s.empty();
        if (!has) return true;
        if (isInt) {
          std::int64_t i = 0;
          if (!_toInt_(s, i)) return false;
          value = static_cast< double >(i);
          return true;
        }
        return parseDecimal(s, value);
      };

      domain               = Configuration::Meta::Domain();
      domain.lowerIncluded = t.front() == '[';
      domain.upperIncluded = t.back() == ']';
      if (!parseBound(t.substr(1, comma - 1), domain.hasLower, domain.lower)
          || !parseBound(t.substr(comma + 1, t.size() - comma - 2),
                         domain.hasUpper,
                         domain.upper)) {
        error = "'" + t + "' has a bound that is not " + (isInt ? "an int" : "a float");
        return false;
      }
      if (domain.hasLower && domain.hasUpper
          && (domain.lower > domain.upper
              || (domain.lower == domain.upper
                  && !(domain.lowerIncluded && domain.upperIncluded)))) {
        error = "the domain '" + t + "' is empty";
        return false;
      }
      domain.text = t;
      return true;
    }

    // parses the `type` part of a metadata line; on failure, gives the reason in error
    bool _parseMetaSpec_(const std::string&   spec,
                         const std::string&   doc,
                         Configuration::Meta& meta,
                         std::string&         error) {
      meta     = Configuration::Meta();
      meta.doc = doc;

      // the type, without the domain of validity that may follow it (e.g. `float]0,1[`)
      const auto cut  = spec.find_first_of("[]");
      const auto type = trim_copy(std::string_view(spec).substr(0, cut));

      if (spec.rfind("deprecated", 0) == 0) {
        const auto rest = trim_copy(std::string_view(spec).substr(10));
        if (rest.rfind("->", 0) == 0) {
          const auto target = trim_copy(std::string_view(rest).substr(2));
          const auto dot    = target.find('.');
          if (dot != std::string::npos && dot > 0 && dot + 1 < target.size()
              && !_hasSpace_(target)) {
            meta.kind          = Configuration::Kind::DEPRECATED;
            meta.targetSection = target.substr(0, dot);
            meta.targetKey     = toLower(target.substr(dot + 1));
            return true;
          }
        }
      } else if (type == "int" || type == "float" || type == "bool" || type == "str") {
        meta.kind = type == "int"   ? Configuration::Kind::INT
                  : type == "float" ? Configuration::Kind::FLOAT
                  : type == "bool"  ? Configuration::Kind::BOOL
                                    : Configuration::Kind::STR;
        if (cut == std::string::npos) return true;
        if (meta.kind == Configuration::Kind::INT || meta.kind == Configuration::Kind::FLOAT) {
          return _parseDomain_(std::string_view(spec).substr(cut),
                               meta.kind == Configuration::Kind::INT,
                               meta.domain,
                               error);
        }
        error = "'" + type + "' cannot have a domain";
        return false;
      } else if (spec.find('|') != std::string::npos) {
        std::size_t prev = 0;
        bool        ok   = true;
        while (true) {
          auto pos = spec.find('|', prev);
          if (pos == std::string::npos) pos = spec.size();
          auto value = trim_copy(std::string_view(spec).substr(prev, pos - prev));
          if (value.empty()) ok = false;
          meta.values.push_back(std::move(value));
          if (pos == spec.size()) break;
          prev = pos + 1;
        }
        if (ok) {
          meta.kind = Configuration::Kind::ENUM;
          return true;
        }
      }
      error = "'" + spec
            + "' is not a valid type (int, float, bool, str, int[min,max], float]min,max[, "
              "v1|v2|..., "
              "deprecated -> section.key)";
      return false;
    }

    std::string _readFile_(const std::string& filename) {
      std::ifstream f(filename, std::ios::binary);
      if (!f) { GUM_ERROR(IOError, "Cannot read file '" << filename << "'.") }
      std::ostringstream content;
      content << f.rdbuf();
      return content.str();
    }

  }   // namespace

  std::string Configuration::Meta::joinedValues() const {
    std::string res;
    for (const auto& v: values) {
      if (!res.empty()) res += '|';
      res += v;
    }
    return res;
  }

  bool Configuration::Meta::Domain::contains(double x) const {
    if ((hasLower || hasUpper) && std::isnan(x)) return false;
    if (hasLower && (x < lower || (x == lower && !lowerIncluded))) return false;
    if (hasUpper && (x > upper || (x == upper && !upperIncluded))) return false;
    return true;
  }

  std::string Configuration::kindName(Kind kind) {
    switch (kind) {
      case Kind::INT : return "int";
      case Kind::FLOAT : return "float";
      case Kind::BOOL : return "bool";
      case Kind::ENUM : return "enum";
      case Kind::DEPRECATED : return "deprecated";
      default : return "str";
    }
  }

  // ==========================================================================
  // construction / parsing
  // ==========================================================================

  Configuration::Configuration(std::string_view defaultsText, std::string_view origin) :
      Configuration(std::initializer_list< DefaultsSource >{DefaultsSource{defaultsText, origin}}) {
  }

  Configuration::Configuration(std::initializer_list< DefaultsSource > sources) {
    for (const auto& source: sources) {
      if (!source.text.empty()) _parse_(source.text, source.origin);
    }
    _groupBySection_();
    _checkSchema_();
    for (std::size_t i = 0; i < _nbEpsilons_; ++i)
      _epsilonPos_[i] = _tryFind_("core", _epsilonKeys_[i]);
    _refreshEpsilons_();
  }

  void Configuration::_parse_(std::string_view text, std::string_view origin) {
    const auto lines = _lines_(text);

    // a section can be reopened by another source but not inside the same one
    std::vector< std::string > openedHere;

    bool        inSection = false;
    std::string section;
    bool        pending = false;
    Meta        pendingMeta;
    std::size_t numline = 0;

    auto err = [&](const std::string& msg) { _syntaxError_(msg, origin, numline); };

    for (const auto& rawLine: lines) {
      ++numline;
      const auto  trimmed = trim_copy(rawLine);
      std::string key, value, name;

      const bool hasKeyValue = !_isCommentOrBlank_(trimmed) && _splitKeyValue_(trimmed, key, value);
      const bool isKeyLine   = hasKeyValue && _isIdentifier_(key);

      if (pending && !isKeyLine) {
        err("metadata line must be immediately followed by a 'key = value' line");
      }

      if (_isMetaLine_(trimmed)) {
        const auto payload = trim_copy(std::string_view(trimmed).substr(3));
        const auto sep     = payload.find("::");
        if (sep == std::string::npos) {
          err("metadata line must follow the syntax '### type :: documentation'");
        }
        const auto spec = trim_copy(std::string_view(payload).substr(0, sep));
        const auto doc  = trim_copy(std::string_view(payload).substr(sep + 2));
        if (doc.empty()) err("missing documentation in metadata line");
        std::string error;
        if (!_parseMetaSpec_(spec, doc, pendingMeta, error)) err(error);
        pending = true;
        continue;
      }

      if (_isCommentOrBlank_(trimmed)) continue;

      if (_isSectionLine_(trimmed, name)) {
        if (std::find(openedHere.begin(), openedHere.end(), name) != openedHere.end()) {
          err("duplicate section '" + name + "'");
        }
        openedHere.push_back(name);
        if (!_hasSection_(name)) _sections_.push_back(name);
        section   = name;
        inSection = true;
        continue;
      }

      if (!inSection) err("key outside of any section");
      if (!hasKeyValue) err("malformed line '" + trimmed + "'");

      key = toLower(key);
      if (const auto first = _tryFind_(section, key); first != _npos_) {
        err("duplicate key '" + section + "." + key + "' (already defined in "
            + _items_[first].origin + ")");
      }

      Item item;
      item.section = section;
      item.key     = key;
      item.value   = value;
      item.origin  = std::string(origin);
      if (pending) {
        item.meta = pendingMeta;
        pending   = false;
      } else {
        item.meta.doc = std::string(_undocumented_);
      }
      _index_[_indexKey_(section, key)] = _items_.size();
      _items_.push_back(std::move(item));
    }

    if (pending) err("metadata line at end of file");
  }

  void Configuration::_groupBySection_() {
    // a section opened by several sources has its items spread in _items_: gather them, keeping
    // the order of the sections and, inside a section, the order of reading
    std::vector< Item > grouped;
    grouped.reserve(_items_.size());
    for (const auto& section: _sections_) {
      for (auto& item: _items_) {
        if (item.section == section) grouped.push_back(std::move(item));
      }
    }
    _items_ = std::move(grouped);
    _index_.clear();
    for (std::size_t pos = 0; pos < _items_.size(); ++pos) {
      _index_[_indexKey_(_items_[pos].section, _items_[pos].key)] = pos;
    }
  }

  void Configuration::_checkSchema_() {
    for (auto& item: _items_) {
      if (item.meta.kind == Kind::DEPRECATED) {
        const auto pos = _tryFind_(item.meta.targetSection, item.meta.targetKey);
        if (pos == _npos_) {
          GUM_SYNTAX_ERROR("deprecated key '"
                               << item.section << "." << item.key << "' redirects to unknown key '"
                               << item.meta.targetSection << "." << item.meta.targetKey << "'",
                           item.origin,
                           0,
                           0)
        }
        if (_items_[pos].meta.kind == Kind::DEPRECATED) {
          GUM_SYNTAX_ERROR("deprecated key '" << item.section << "." << item.key
                                              << "' redirects to deprecated key '"
                                              << item.meta.targetSection << "."
                                              << item.meta.targetKey << "'",
                           item.origin,
                           0,
                           0)
        }
      } else {
        std::string canonical, error;
        if (!_validate_(item, item.value, canonical, error)) {
          GUM_ERROR(InvalidArgument, "[" << item.origin << "] " << error)
        }
        item.value        = canonical;
        item.defaultValue = canonical;
      }
    }
  }

  // ==========================================================================
  // lookup
  // ==========================================================================

  std::string Configuration::_indexKey_(std::string_view section, std::string_view key) {
    std::string res(section);
    res += '\0';
    res += toLower(key);
    return res;
  }

  std::size_t Configuration::_tryFind_(std::string_view section, std::string_view key) const {
    const auto it = _index_.find(_indexKey_(section, key));
    return it == _index_.end() ? _npos_ : it->second;
  }

  bool Configuration::_hasSection_(std::string_view section) const {
    return std::find(_sections_.begin(), _sections_.end(), section) != _sections_.end();
  }

  std::size_t Configuration::_find_(std::string_view section, std::string_view key) const {
    const auto pos = _tryFind_(section, key);
    if (pos != _npos_) return pos;
    if (!_hasSection_(section)) {
      GUM_SYNTAX_ERROR("Section '" << section << "' unknown in configuration.", "", 0, 0)
    }
    GUM_SYNTAX_ERROR("Key '" << section << "." << key << "' unknown in configuration.", "", 0, 0)
  }

  void Configuration::_queue_(Notice::Kind kind, std::string message) const {
    std::lock_guard< std::mutex > lock(_noticeMutex_);
    // a notice already waiting is not queued twice (e.g. a deprecated key read in a loop)
    const auto same = [&](const Notice& n) { return n.kind == kind && n.message == message; };
    if (std::none_of(_notices_.begin(), _notices_.end(), same)) {
      _notices_.push_back(Notice{kind, std::move(message)});
    }
  }

  std::size_t Configuration::_resolve_(std::size_t pos, std::vector< Notice >* sink) const {
    const auto& item = _items_[pos];
    if (item.meta.kind != Kind::DEPRECATED) return pos;
    Notice notice{Notice::Kind::DEPRECATION,
                  "'" + item.section + "." + item.key + "' is deprecated (" + item.meta.doc
                      + "), use '" + item.meta.targetSection + "." + item.meta.targetKey
                      + "' instead."};
    if (sink != nullptr) sink->push_back(std::move(notice));
    else _queue_(notice.kind, std::move(notice.message));
    return _tryFind_(item.meta.targetSection, item.meta.targetKey);   // checked by _checkSchema_
  }

  std::pair< std::string, Configuration::Kind >
      Configuration::_typedRaw_(std::string_view section, std::string_view key) const {
    std::shared_lock lock(_mutex_);
    const auto&      item = _items_[_resolve_(_find_(section, key))];
    return {item.value, item.meta.kind};
  }

  // ==========================================================================
  // validation
  // ==========================================================================

  bool Configuration::_validate_(const Item&      item,
                                 std::string_view value,
                                 std::string&     canonical,
                                 std::string&     error) {
    const auto name = item.section + "." + item.key;
    const auto s    = trim_copy(value);
    canonical       = s;

    switch (item.meta.kind) {
      case Kind::INT : {
        std::int64_t res = 0;
        if (!_toInt_(s, res)) {
          error = "'" + s + "' must be an int for '" + name + "'.";
          return false;
        }
        if (!item.meta.domain.contains(static_cast< double >(res))) {
          error = "'" + s + "' must be in " + item.meta.domain.text + " for '" + name + "'.";
          return false;
        }
        return true;
      }
      case Kind::FLOAT : {
        double res = 0;
        if (!parseDecimal(s, res)) {
          error = "'" + s + "' must be a float for '" + name + "'.";
          return false;
        }
        if (!item.meta.domain.contains(res)) {
          error = "'" + s + "' must be in " + item.meta.domain.text + " for '" + name + "'.";
          return false;
        }
        return true;
      }
      case Kind::BOOL : {
        const auto low = toLower(s);
        if (low == "true" || low == "1" || low == "on" || low == "yes") {
          canonical = "True";
          return true;
        }
        if (low == "false" || low == "0" || low == "off" || low == "no") {
          canonical = "False";
          return true;
        }
        error
            = "'" + s + "' must be a boolean (False/True, 0/1, Off/On, No/Yes) for '" + name + "'.";
        return false;
      }
      case Kind::ENUM : {
        const auto low = toLower(s);
        for (const auto& v: item.meta.values) {
          if (toLower(v) == low) {
            canonical = v;
            return true;
          }
        }
        error = "'" + s + "' must be one of " + item.meta.joinedValues() + " for '" + name + "'.";
        return false;
      }
      default : canonical = std::string(value); return true;
    }
  }

  // ==========================================================================
  // accessors
  // ==========================================================================

  std::string Configuration::get(std::string_view section, std::string_view key) const {
    std::shared_lock lock(_mutex_);
    return _items_[_resolve_(_find_(section, key))].value;
  }

  Configuration::Value Configuration::getValue(std::string_view section,
                                               std::string_view key) const {
    const auto [value, kind] = _typedRaw_(section, key);
    switch (kind) {
      case Kind::BOOL : return value == "True";
      case Kind::INT : return static_cast< std::int64_t >(std::strtoll(value.c_str(), nullptr, 10));
      case Kind::FLOAT : {
        double res = 0;
        parseDecimal(value, res);
        return res;
      }
      default : return value;
    }
  }

  bool Configuration::has(std::string_view section, std::string_view key) const {
    std::shared_lock lock(_mutex_);
    return _tryFind_(section, key) != _npos_;
  }

  Configuration::Meta Configuration::meta(std::string_view section, std::string_view key) const {
    std::shared_lock lock(_mutex_);
    return _items_[_find_(section, key)].meta;
  }

  std::string Configuration::doc(std::string_view section, std::string_view key) const {
    std::shared_lock lock(_mutex_);
    const auto&      m = _items_[_find_(section, key)].meta;
    switch (m.kind) {
      case Kind::DEPRECATED :
        return "deprecated -> " + m.targetSection + "." + m.targetKey + " :: " + m.doc;
      case Kind::ENUM : return m.joinedValues() + " :: " + m.doc;
      default : return kindName(m.kind) + m.domain.text + " :: " + m.doc;
    }
  }

  std::vector< Configuration::SchemaEntry > Configuration::schema() const {
    std::shared_lock           lock(_mutex_);
    std::vector< SchemaEntry > res;
    res.reserve(_items_.size());
    for (const auto& item: _items_) {
      res.push_back(SchemaEntry{item.section, item.key, item.meta, item.defaultValue});
    }
    return res;
  }

  std::vector< std::string > Configuration::sections() const {
    std::shared_lock lock(_mutex_);
    return _sections_;
  }

  // ==========================================================================
  // modifiers
  // ==========================================================================

  void Configuration::_setAt_(std::size_t pos, std::string_view value) {
    std::string canonical, error;
    if (!_validate_(_items_[pos], value, canonical, error)) { GUM_ERROR(InvalidArgument, error) }
    _items_[pos].value = std::move(canonical);
    // the atomics only depend on the three tolerances
    if (std::find(_epsilonPos_.begin(), _epsilonPos_.end(), pos) != _epsilonPos_.end()) {
      _refreshEpsilons_();
    }
  }

  void Configuration::_refreshEpsilons_() {
    for (std::size_t i = 0; i < _nbEpsilons_; ++i) {
      double value = 0;
      if (_epsilonPos_[i] != _npos_ && parseDecimal(_items_[_epsilonPos_[i]].value, value)) {
        _epsilons_[i].store(value, std::memory_order_relaxed);
      }
    }
  }

  void Configuration::set(std::string_view section,
                          std::string_view key,
                          std::string_view value,
                          bool             noHook) {
    std::size_t pos = 0;
    std::string previous;
    {
      std::unique_lock lock(_mutex_);
      pos      = _resolve_(_find_(section, key));
      previous = _items_[pos].value;
      _setAt_(pos, value);
    }
    if (noHook) return;

    try {
      runHooks();
    } catch (...) {
      {
        std::unique_lock lock(_mutex_);
        _items_[pos].value = std::move(previous);
        _refreshEpsilons_();
      }
      // bring back the state driven by the hooks; the original exception is the one to report
      try {
        runHooks();
      } catch (...) {}
      throw;
    }
  }

  Configuration::Snapshot Configuration::_snapshot_() const {
    Snapshot res;
    res.reserve(_items_.size());
    for (const auto& item: _items_)
      res.push_back(item.value);
    return res;
  }

  void Configuration::_restore_(const Snapshot& snapshot) {
    for (std::size_t i = 0; i < _items_.size(); ++i)
      _items_[i].value = snapshot[i];
    _refreshEpsilons_();
  }

  void Configuration::reset() {
    {
      std::unique_lock lock(_mutex_);
      for (auto& item: _items_)
        if (item.meta.kind != Kind::DEPRECATED) item.value = item.defaultValue;
      _refreshEpsilons_();
    }
    runHooks();
  }

  void Configuration::push() {
    std::unique_lock lock(_mutex_);
    _stack_.push_back(_snapshot_());
  }

  void Configuration::pop() {
    {
      std::unique_lock lock(_mutex_);
      if (_stack_.empty()) { GUM_ERROR(OperationNotAllowed, "No configuration to pop.") }
      _restore_(_stack_.back());
      _stack_.pop_back();
    }
    runHooks();
  }

  // ==========================================================================
  // files and display
  // ==========================================================================

  void Configuration::load(const std::string& filename) {
    const auto lines = _lines_(_readFile_(filename));

    // The whole file is read before anything is changed: a malformed line leaves the values (and
    // thus the state driven by the hooks) untouched.
    std::vector< std::pair< std::size_t, std::string > > changes;   // (position, canonical value)
    std::vector< Notice >                                notices;
    auto                                                 note = [&](std::string message) {
      notices.push_back(Notice{Notice::Kind::FILE, "[" + filename + "] " + std::move(message)});
    };
    {
      std::shared_lock lock(_mutex_);
      std::string      section;
      bool             knownSection = false;
      std::size_t      numline      = 0;

      for (const auto& rawLine: lines) {
        ++numline;
        const auto  trimmed = trim_copy(rawLine);
        std::string name, key, value;
        if (_isCommentOrBlank_(trimmed)) continue;

        if (_isSectionLine_(trimmed, name)) {
          section      = name;
          knownSection = _hasSection_(section);
          if (!knownSection) { note("Section '" + section + "' does not exist."); }
          continue;
        }

        if (section.empty() || !_splitKeyValue_(trimmed, key, value)) {
          _syntaxError_("malformed line '" + trimmed + "'", filename, numline);
        }
        if (!knownSection) continue;

        const auto pos = _tryFind_(section, key);
        if (pos == _npos_) {
          note("Option '" + section + "." + toLower(key) + "' does not exist.");
          continue;
        }
        const auto target = _resolve_(pos, &notices);

        std::string canonical, error;
        if (_validate_(_items_[target], value, canonical, error)) {
          changes.emplace_back(target, std::move(canonical));
        } else {
          note("Invalid value for '" + section + "." + toLower(key) + "': " + error);
        }
      }
    }

    {
      std::unique_lock lock(_mutex_);
      for (auto& [pos, value]: changes)
        _items_[pos].value = std::move(value);
      _refreshEpsilons_();
    }
    for (auto& notice: notices)
      _queue_(notice.kind, std::move(notice.message));
    runHooks();
  }

  void Configuration::save(const std::string& filename) const {
    const auto    content = diff();
    std::ofstream f(filename, std::ios::binary | std::ios::trunc);
    if (!f) { GUM_ERROR(IOError, "Cannot write file '" << filename << "'.") }
    f << content << '\n';
    if (!f) { GUM_ERROR(IOError, "Cannot write file '" << filename << "'.") }
  }

  std::string Configuration::_render_(const std::function< bool(const Item&) >& keep) const {
    std::shared_lock lock(_mutex_);
    std::string      res;
    // the keys of a section are contiguous in _items_ (a duplicate section is refused by _parse_)
    const std::string* current = nullptr;   // section of the last rendered key
    for (const auto& item: _items_) {
      if (item.meta.kind == Kind::DEPRECATED || !keep(item)) continue;
      if (current == nullptr || *current != item.section) {
        if (current != nullptr) res += '\n';
        res += "[" + item.section + "]";
        current = &item.section;
      }
      res += "\n  " + item.key + " = " + item.value;
    }
    return res;
  }

  std::string Configuration::grep(std::string_view search) const {
    const auto low = toLower(search);
    return _render_([&](const Item& item) {
      return contains(toLower(item.section), low) || contains(item.key, low);
    });
  }

  std::string Configuration::diff() const {
    return _render_([](const Item& item) { return item.value != item.defaultValue; });
  }

  std::string Configuration::toString() const {
    return _render_([](const Item&) { return true; });
  }

  std::string Configuration::describe() const {
    const auto changes = diff();
    return changes.empty() ? "# no customized property\n" + toString() : changes;
  }

  // ==========================================================================
  // hooks and notices
  // ==========================================================================

  void Configuration::addHook(Hook hook) {
    std::unique_lock lock(_mutex_);
    _hooks_.push_back(std::move(hook));
  }

  void Configuration::runHooks() const {
    std::vector< Hook > hooks;
    {
      std::shared_lock lock(_mutex_);
      hooks = _hooks_;
    }
    for (const auto& hook: hooks)
      hook(*this);
  }

  std::vector< Configuration::Notice > Configuration::takeNotices() {
    std::lock_guard< std::mutex > lock(_noticeMutex_);
    return std::exchange(_notices_, {});
  }

  // ==========================================================================
  // singleton
  // ==========================================================================

  std::string Configuration::userFileName() { return GUM_CONFIGURATION_USER_FILE; }

  void Configuration::_loadUserFile_() {
    try {
      const auto filename = userFileName();
      if (std::filesystem::is_regular_file(filename)) load(filename);
    } catch (const std::exception& e) {
      _queue_(Notice::Kind::FILE, std::string("[") + userFileName() + "] " + e.what());
    }
  }

  Configuration& config() { return Configuration::instance(); }

  Configuration& Configuration::instance() {
    // built, completed by the user file and applied once (thread-safe), the first time it is needed
    static Configuration& instanceRef = []() -> Configuration& {
      static Configuration global{
          {configurationEmbeddedDefaults(), "defaults.ini (aGrUM)"},
          {configurationEmbeddedPyAgrumDefaults(), "defaults.ini (pyAgrum)"}};
      // the number of threads is the only [core] key acting directly on the library; a negative
      // value gives back what the machine (or OMP_NUM_THREADS) provides, read before any change
      global.addHook([machine = gum::getMaxNumberOfThreads()](const Configuration& c) {
        const auto n = c.typed< std::int64_t >("core", "maxNumberOfThreads");
        gum::setNumberOfThreads(n < 0 ? machine : static_cast< unsigned int >(n));
      });
      global._loadUserFile_();
      try {
        global.runHooks();
      } catch (const std::exception& e) {
        // e.g. a number of threads that cannot be honoured: the library must stay usable
        global._queue_(Notice::Kind::FILE, std::string("hook failed: ") + e.what());
      }
      return global;
    }();
    return instanceRef;
  }

  namespace {
    // loads the configuration (and applies its hooks) as soon as the library is loaded, so that the
    // user file acts even on a program which never reads the configuration. Beware: with a static
    // library, this object is only kept if something else references this translation unit.
    const struct ConfigurationInitializer {
      ConfigurationInitializer() {
        try {
          Configuration::instance();
        } catch (...) {}   // never let an exception escape a static initializer
      }
    } _configurationInitializer_;
  }   // namespace

}   // namespace gum
