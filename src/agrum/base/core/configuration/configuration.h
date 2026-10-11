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
 * @brief Typed, documented, sectioned configuration singleton read from an .ini content.
 *
 * The structure of the configuration (sections, keys, default values, types and one-line
 * documentation) is given by a "defaults" ini text. Each key is documented by a metadata line
 * placed immediately above it:
 *
 *     ### type :: one-line documentation
 *     key = value
 *
 * where type is one of `int`, `float`, `bool`, `str`, an enumeration `v1|v2|...` or a
 * deprecation `deprecated -> section.key`. Lines starting with `#` (but not `###`) or `;` are
 * free comments.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_CONFIGURATION_H
#define GUM_CONFIGURATION_H

#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <agrum/agrum.h>

#include <shared_mutex>
#include <string_view>
#include <unordered_map>

namespace gum {

  /**
   * @class Configuration
   * @headerfile configuration.h <agrum/base/core/configuration/configuration.h>
   * @ingroup utilities_group
   * @brief Sectioned key/value configuration with typed and documented keys.
   *
   * Values are stored as strings and canonicalized according to the declared type of their key:
   * booleans are stored as `True` or `False` (accepted spellings, whatever the case: TRUE, 1, ON,
   * YES / FALSE, 0, OFF, NO), enumerated values with the casing declared in the defaults.
   *
   * Section names are case-sensitive, key names are not (they are stored in lowercase).
   *
   * Thread-safety: every public method is thread-safe (readers run concurrently, writers are
   * exclusive). Values are returned by copy. Hooks are run outside of any lock, so a hook may
   * freely read the configuration.
   *
   * See also the howto @ref using_configuration (syntax of the defaults, hooks, notices, examples).
   *
   * Errors:
   *  - gum::SyntaxError for a malformed defaults text, or an unknown section/key,
   *  - gum::InvalidArgument for a value that does not respect the declared type of its key,
   *  - gum::OperationNotAllowed for pop() on an empty stack,
   *  - gum::IOError for an unreadable or unwritable file.
   */
  class GUM_SHARED_PUBLIC Configuration {
    public:
    /// the declared type of a key
    enum class Kind : unsigned char { INT, FLOAT, BOOL, STR, ENUM, DEPRECATED };

    /// metadata attached to a key by a `###` line
    struct Meta {
      /// domain of validity of an int or float key, written after the type in the `###` line:
      /// `float]0,1[`, `int[1,]`, `int[0,100]`. A bracket turned toward the bound includes it, turned
      /// away excludes it ; an omitted bound is unbounded.
      struct Domain {
        bool        hasLower{false};
        bool        hasUpper{false};
        bool        lowerIncluded{true};
        bool        upperIncluded{true};
        double      lower{0.};
        double      upper{0.};
        std::string text;   ///< the domain as written, without blanks (e.g. `]0,1[`) ; empty if none

        /// @return true if x belongs to the domain (a domain without bound contains every number).
        bool contains(double x) const;
      };

      Kind                       kind{Kind::STR};
      Domain                     domain;          ///< domain of validity (kind == INT or FLOAT)
      std::vector< std::string > values;          ///< allowed values (kind == ENUM)
      std::string                doc;             ///< one-line documentation
      std::string                targetSection;   ///< replacement key (kind == DEPRECATED)
      std::string                targetKey;       ///< replacement key (kind == DEPRECATED)

      /// @return the allowed values joined by '|' (kind == ENUM), empty otherwise.
      std::string joinedValues() const;
    };

    /// a value converted according to the declared type of its key (see getValue())
    using Value = std::variant< bool, std::int64_t, double, std::string >;

    /// @return the name of a kind: `int`, `float`, `bool`, `str`, `enum` or `deprecated`.
    static std::string kindName(Kind kind);

    /// one line of the schema (see schema())
    struct SchemaEntry {
      std::string section;
      std::string key;
      Meta        meta;
      std::string defaultValue;   ///< empty for a deprecated key
    };

    /// a message that cannot be an exception (the operation went on), see takeNotices()
    struct Notice {
      enum class Kind : unsigned char { DEPRECATION, FILE };
      Notice::Kind kind;
      std::string  message;
    };

    using Hook = std::function< void(const Configuration&) >;

    /// @name Constructors, destructor and singleton
    /// @{

    /**
     * @brief Builds a configuration from a defaults text.
     * @param defaultsText the content of a defaults ini.
     * @param origin the name used in error messages to designate the defaults text.
     * @throws SyntaxError if the text or its metadata are malformed, or if a deprecated key points
     *         to an unknown (or itself deprecated) key.
     * @throws InvalidArgument if a default value does not respect its own declared type.
     */
    explicit Configuration(std::string_view defaultsText, std::string_view origin = "defaults.ini");

    Configuration(const Configuration&)            = delete;
    Configuration& operator=(const Configuration&) = delete;
    ~Configuration()                               = default;

    /**
     * @brief The global configuration.
     *
     * Built at first call from the defaults embedded in the library (those of aGrUM and, for a
     * pyAgrum build, those of pyAgrum). If the user file (see userFileName()) exists in the current
     * directory, it is loaded: it can change the value of a key but never creates one (problems are
     * queued in the notices, see takeNotices()).
     */
    static Configuration& instance();

    /// @return the name of the user file loaded from the current directory by instance().
    static std::string userFileName();

    /// @}

    /// @name Accessors
    /// @{

    /**
     * @brief Value of section.key, as a string.
     * @throws SyntaxError if the section or the key does not exist.
     */
    std::string get(std::string_view section, std::string_view key) const;

    /**
     * @brief Value of section.key converted according to its declared type.
     * @tparam T bool (for a bool key), int64_t (int key), double (float or int key) or
     *         std::string (any key).
     * @throws SyntaxError if the section or the key does not exist.
     * @throws InvalidArgument if T is not compatible with the declared type of the key.
     */
    template < typename T >
    T typed(std::string_view section, std::string_view key) const;

    /**
     * @brief Value of section.key converted according to its declared type (following a
     * deprecation).
     * @return a bool (bool key), an int64_t (int key), a double (float key) or a std::string (str
     * and enum keys).
     * @throws SyntaxError if the section or the key does not exist.
     */
    Value getValue(std::string_view section, std::string_view key) const;

    /// @return true if section.key exists.
    bool has(std::string_view section, std::string_view key) const;

    /// @return the metadata of section.key.
    Meta meta(std::string_view section, std::string_view key) const;

    /// @return `type :: documentation` for section.key (`str :: (undocumented)` if none).
    std::string doc(std::string_view section, std::string_view key) const;

    /// @return the whole schema (in the order of the defaults).
    std::vector< SchemaEntry > schema() const;

    /// @return the names of the sections, in the order of the defaults.
    std::vector< std::string > sections() const;

    /// @}

    /// @name Modifiers
    /// @{

    /**
     * @brief Sets section.key.
     *
     * Setting a deprecated key sets its replacement and queues a deprecation notice.
     * If a hook throws, the previous value is restored (the hooks are run again, their exceptions
     * being ignored) and the exception is propagated.
     * @param noHook if true, the hooks are not run.
     * @throws SyntaxError if the section or the key does not exist.
     * @throws InvalidArgument if the value does not respect the declared type of the key.
     */
    void set(std::string_view section,
             std::string_view key,
             std::string_view value,
             bool             noHook = false);

    /// Back to the defaults. Runs the hooks.
    void reset();

    /// Pushes the current values on a stack.
    void push();

    /**
     * @brief Restores the values on top of the stack. Runs the hooks.
     * @throws OperationNotAllowed if the stack is empty.
     */
    void pop();

    /// @}

    /// @name Files and display
    /// @{

    /**
     * @brief Loads a user file and changes the values it contains. Runs the hooks.
     *
     * Unknown sections, unknown keys, invalid values and deprecated keys do not stop the loading:
     * they are queued as notices and the file is never modified (a deprecated key is applied to its
     * replacement; save() writes the replacement, not the deprecated key). Only a malformed line
     * stops the loading, and nothing is changed in this case.
     * @throws IOError if the file cannot be read.
     * @throws SyntaxError if the file contains a malformed line.
     */
    void load(const std::string& filename);

    /**
     * @brief Writes the difference with the defaults (see diff()) in a file.
     * @throws IOError if the file cannot be written.
     */
    void save(const std::string& filename) const;

    /// @return the visible keys (all but the deprecated ones) containing @p search, with their
    /// section; a matching section name selects the whole section.
    std::string grep(std::string_view search) const;

    /// @return the visible keys whose value differs from the default.
    std::string diff() const;

    /// @return all the visible keys.
    std::string toString() const;

    /// @return diff() if the configuration differs from the defaults, otherwise a comment
    /// saying so followed by toString().
    std::string describe() const;

    /// @}

    /// @name Numerical tolerances
    /// @{

    /**
     * @brief The coarse tolerance (key `core.epsilonCoarse`, 1e-2 by default).
     *
     * The three tolerances of aGrUM are read without any lock nor allocation (an atomic load),
     * so that they can be used in loops. Their domains do not overlap (coarse in [1e-3,1e-1],
     * standard in [1e-8,1e-5], fine in [1e-11,1e-9]), so that fine < standard < coarse always holds : a
     * value out of the domain is refused as any invalid value (see set() and load()).
     */
    double epsilonCoarse() const noexcept;

    /// @return the standard tolerance (key `core.epsilonStandard`, 1e-6 by default).
    double epsilonStandard() const noexcept;

    /// @return the fine tolerance (key `core.epsilonFine`, 1e-10 by default).
    double epsilonFine() const noexcept;

    /// @}

    /// @name Hooks and notices
    /// @{

    /// Adds a function called after each change (see set(), reset(), pop(), load()).
    void addHook(Hook hook);

    /// Runs the hooks (outside of any lock).
    void runHooks() const;

    /// @return the queued notices (deprecations, problems found when loading a file) and empties
    /// the queue.
    std::vector< Notice > takeNotices();

    /// @}

    private:
    struct Item {
      std::string section;
      std::string key;
      Meta        meta;
      std::string value;
      std::string defaultValue;
    };

    using Snapshot = std::vector< std::string >;   // values, in the order of items_

    mutable std::shared_mutex  _mutex_;
    std::vector< Item >        _items_;
    std::vector< std::string > _sections_;
    std::unordered_map< std::string, std::size_t >
                            _index_;   // "section\0key" -> position in _items_
    std::vector< Snapshot > _stack_;
    std::vector< Hook >     _hooks_;

    // the numerical tolerances (core.epsilonCoarse, ...) are copied in atomics, under the lock,
    // each time a value changes; their positions in _items_ are _npos_ if the schema does not have
    // them
    static constexpr std::size_t                      _nbEpsilons_ = 3;
    std::array< std::atomic< double >, _nbEpsilons_ > _epsilons_{{1e-2, 1e-6, 1e-10}};
    std::array< std::size_t, _nbEpsilons_ >           _epsilonPos_{};

    mutable std::mutex            _noticeMutex_;
    mutable std::vector< Notice > _notices_;

    void _parse_(std::string_view text, std::string_view origin);
    // checks the consistency of the schema and canonicalizes the default values
    void _checkSchema_(std::string_view origin);

    static constexpr std::size_t _npos_ = static_cast< std::size_t >(-1);

    static std::string _indexKey_(std::string_view section, std::string_view key);
    // position of section.key, _npos_ if unknown
    std::size_t _tryFind_(std::string_view section, std::string_view key) const;
    // position of section.key; throws SyntaxError if unknown
    std::size_t _find_(std::string_view section, std::string_view key) const;
    bool        _hasSection_(std::string_view section) const;
    // position of the item that actually carries the value (follows a deprecation and gives a
    // notice: queued, or appended to sink if not null)
    std::size_t _resolve_(std::size_t pos, std::vector< Notice >* sink = nullptr) const;
    // value and declared kind of the item that actually carries the value of section.key
    std::pair< std::string, Kind > _typedRaw_(std::string_view section, std::string_view key) const;
    // checks value against the declared type of item; on success gives its canonical form,
    // otherwise gives the reason in error
    static bool _validate_(const Item&      item,
                           std::string_view value,
                           std::string&     canonical,
                           std::string&     error);
    // sets the value of _items_[pos]; throws InvalidArgument if it does not respect the declared
    // type
    void _setAt_(std::size_t pos, std::string_view value);
    // loads the user file of instance(), never throws
    void        _loadUserFile_();
    std::string _render_(const std::function< bool(const Item&) >& keep) const;
    Snapshot    _snapshot_() const;
    void        _restore_(const Snapshot& snapshot);
    // copies the values of the numerical tolerances in their atomics (needs the exclusive lock)
    void _refreshEpsilons_();
    void _queue_(Notice::Kind kind, std::string message) const;
  };

  /// @brief A short name for Configuration::instance() : `gum::config().epsilonStandard()`.
  Configuration& config();

}   // namespace gum

#include <agrum/base/core/configuration/configuration_tpl.h>

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/configuration/configuration_inl.h>
#endif   // GUM_NO_INLINE

#endif   /* GUM_CONFIGURATION_H */
