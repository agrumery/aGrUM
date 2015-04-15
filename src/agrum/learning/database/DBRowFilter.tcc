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
/** @file
 * @brief Template implementation of DBRowFilters
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::DBRowFilter(
        const DBHandler &handler, const TranslatorSet &translator_set,
        const GeneratorSet &generator_set,
        unsigned long initialization_range) noexcept
        : __handler(handler),
          __translator_set(translator_set),
          __generator_set(generator_set) {
      if (__translator_set.requiresInitialization()) {
        __initCellFilters(initialization_range);
      }
      GUM_CONSTRUCTOR(DBRowFilter);
    }

    /// copy constructor
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::DBRowFilter(
        const DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &filter) noexcept
        : __handler(filter.__handler),
          __translator_set(filter.__translator_set),
          __generator_set(filter.__generator_set) {
      GUM_CONS_CPY(DBRowFilter);
    }

    /// move constructor
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::DBRowFilter(
        DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &&filter) noexcept
        : __handler(std::move(filter.__handler)),
          __translator_set(std::move(filter.__translator_set)),
          __generator_set(std::move(filter.__generator_set)) {
      GUM_CONS_MOV(DBRowFilter);
    }

    /// destructor
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE DBRowFilter<DBHandler, TranslatorSet,
                       GeneratorSet>::~DBRowFilter() noexcept {
      GUM_DESTRUCTOR(DBRowFilter);
    }

    /// copy operator
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &
        DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::
        operator=(
            const DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &filter) {
      if (this != &filter) {
        __handler = filter.__handler;
        __translator_set = filter.__translator_set;
        __generator_set = filter.__generator_set;
      }
      return *this;
    }

    /// move operator
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &
        DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::
        operator=(DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &&filter) {
      if (this != &filter) {
        __handler = std::move(filter.__handler);
        __translator_set = std::move(filter.__translator_set);
        __generator_set = std::move(filter.__generator_set);
      }
      return *this;
    }

    /// returns true if there are still rows that can be output by the RowFilter
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE bool
    DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::hasRows() noexcept {
      return __handler.hasRows() || __generator_set.hasRows();
    }

    /// initialize the cell filters by parsing once the database
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    void DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::__initCellFilters(
        unsigned long db_range) {
      // keep track of the previous range to restore it later
      std::pair<unsigned long, unsigned long> previous_range = __handler.range();

      // set the handler to the appropriate range
      if (__handler.size() != db_range) {
        if (__handler.DBSize() < db_range) {
          __handler.setRange(0, __handler.DBSize());
        } else {
          __handler.setRange(0, db_range);
        }
      } else {
        __handler.reset();
      }

      // initialize the cell filters
      while (__handler.hasRows()) {
        // get the next row to parse from the database
        __translator_set.setInputRow(__handler.row());
        __handler.nextRow();

        // initialize the cell filters with this row
        __translator_set.initialize();
      }
      __translator_set.postInitialize();

      // restore the previous range
      __handler.setRange(previous_range.first, previous_range.second);
    }

    /// returns a new output row with its corresponding weight
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE FilteredRow &DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::row() {
      while (!__generator_set.hasRows()) {
        if (!__handler.hasRows()) {
          GUM_ERROR(NotFound, "There are no more rows to parse");
        }

        // get the next row to parse from the database
        __translator_set.setInputRow(__handler.row());
        __handler.nextRow();

        // translate the database cells
        __translator_set.translate();

        // pass the translation to the filtered row generator
        __generator_set.setInputRow(__translator_set.outputRow());
      }

      return __generator_set.generate();
    }

    /// resets the filter
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE void DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::reset() {
      __handler.reset();
      __generator_set.reset();
    }

    // returns the handler used by the filter
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE DBHandler &
    DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::handler() noexcept {
      return __handler;
    }

    /** @brief returns the number of modalities of the variables, as stored
     * into the cell filters */
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE std::vector<unsigned int>
    DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::modalities() const {
      std::vector<unsigned int> res;
      __translator_set.modalities(res);
      return res;
    }

    /// returns the names of the variables
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE const std::vector<std::string> &
    DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::variableNames() const
        noexcept {
      return __handler.variableNames();
    }

    /// returns the number of variables
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE unsigned int
    DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::nbVariables() const
        noexcept {
      return __handler.variableNames().size();
    }

    /// returns the translator set that is actually used
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE const TranslatorSet &
    DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::translatorSet() const
        noexcept {
      return __translator_set;
    }

    /// returns the generator set that is actually used
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE const GeneratorSet &
    DBRowFilter<DBHandler, TranslatorSet, GeneratorSet>::generatorSet() const
        noexcept {
      return __generator_set;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
