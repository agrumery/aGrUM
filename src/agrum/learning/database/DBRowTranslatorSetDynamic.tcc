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
 * @brief The "non-meta-programming" set of translators stored into a row filter
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename Translator>
    INLINE DBRowTranslatorSetDynamic<Translator>::DBRowTranslatorSetDynamic() {
      GUM_CONSTRUCTOR(DBRowTranslatorSetDynamic);
    }

    /// copy constructor
    template <typename Translator>
    DBRowTranslatorSetDynamic<Translator>::DBRowTranslatorSetDynamic(
        const DBRowTranslatorSetDynamic<Translator> &from)
        : __translators(from.__translators), __output_size(from.__output_size),
          __output_row(from.__output_row) {
      // create the translators
      for (unsigned int i = 0, size = __translators.size(); i < size; ++i) {
        __translators[i] = from.__translators[i]->copyFactory();
        __translators[i]->setOutputRow(__output_row);
      }

      GUM_CONS_CPY(DBRowTranslatorSetDynamic);
    }

    /// move constructor
    template <typename Translator>
    INLINE DBRowTranslatorSetDynamic<Translator>::DBRowTranslatorSetDynamic(
        DBRowTranslatorSetDynamic<Translator> &&from)
        : __translators(std::move(from.__translators)),
          __output_size(std::move(from.__output_size)),
          __output_row(std::move(from.__output_row)) {
      from.__translators.clear();
      for (unsigned int i = 0, size = __translators.size(); i < size; ++i) {
        __translators[i]->setOutputRow(__output_row);
      }

      GUM_CONS_MOV(DBRowTranslatorSetDynamic);
    }

    /// destructor
    template <typename Translator>
    INLINE DBRowTranslatorSetDynamic<
        Translator>::~DBRowTranslatorSetDynamic() noexcept {
      for (auto translator : __translators) {
        delete translator;
      }
      GUM_DESTRUCTOR(DBRowTranslatorSetDynamic);
    }

    /// remove all the translators from the vector
    template <typename Translator>
    INLINE void DBRowTranslatorSetDynamic<Translator>::clear() noexcept {
      for (auto translator : __translators) {
        delete translator;
      }
      __translators.clear();
      __output_size = 0;
      __output_row.row().clear();
    }

    /// copy operator
    template <typename Translator>
    DBRowTranslatorSetDynamic<Translator> &DBRowTranslatorSetDynamic<Translator>::
    operator=(const DBRowTranslatorSetDynamic<Translator> &from) {
      if (this != &from) {
        clear();

        __translators = from.__translators;
        __output_size = from.__output_size;
        __output_row = from.__output_row;

        // create the translators
        for (unsigned int i = 0, size = __translators.size(); i < size; ++i) {
          __translators[i] = from.__translators[i]->copyFactory();
          __translators[i]->setOutputRow(__output_row);
        }
      }

      return *this;
    }

    /// move operator
    template <typename Translator>
    DBRowTranslatorSetDynamic<Translator> &DBRowTranslatorSetDynamic<Translator>::
    operator=(DBRowTranslatorSetDynamic<Translator> &&from) {
      if (this != &from) {
        clear();

        __translators = std::move(from.__translators);
        __output_size = std::move(from.__output_size);
        __output_row = std::move(from.__output_row);
        from.__translators.clear();
        for (unsigned int i = 0, size = __translators.size(); i < size; ++i) {
          __translators[i]->setOutputRow(__output_row);
        }
      }

      return *this;
    }

    /// inserts new translators at the end of the vector
    template <typename Translator>
    template <typename Cols, typename ColsIncr>
    void DBRowTranslatorSetDynamic<Translator>::insertTranslator(
        Cols deb_cols, unsigned int nb_times, ColsIncr incr) {
      // we get the contents of deb_cols and incr and we will compute and set the
      // input columns by ourselves
      const unsigned int input_size =
          std::remove_reference<Translator>::type::input_size;
      unsigned int input_cols[input_size];
      unsigned int incr_cols[input_size];
      deb_cols.toArray(input_cols);
      incr.toArray(incr_cols);

      // now insert the translators
      for (unsigned int i = 0; i < nb_times; ++i) {
        // create the translator
        Translator *new_translator = new Translator;
        try {
          __translators.push_back(new_translator);
        } catch (...) {
          delete new_translator;
          __output_row.row().resize(__output_size);
          throw;
        }

        // assign its output columns and rowFilter
        new_translator->setOutputRow(__output_row);
        new_translator->setOutputCols(__output_size);
        __output_size += std::remove_reference<Translator>::type::output_size;

        // assign the input col
        unsigned int *inputs =
            const_cast<unsigned int *>(new_translator->inputCols());
        std::memcpy(inputs, input_cols, input_size * sizeof(unsigned int));
        for (unsigned int j = 0; j < input_size; ++j) {
          inputs[j] = input_cols[j];
          input_cols[j] += incr_cols[j];
        }
      }

      __output_row.row().resize(__output_size);
    }

    /// inserts new translators at the end of the vector
    template <typename Translator>
    template <typename NewTranslator, typename Cols, typename ColsIncr>
    void DBRowTranslatorSetDynamic<Translator>::insertTranslator(
        const NewTranslator &translator, Cols deb_cols, unsigned int nb_times,
        ColsIncr incr) {
      // we get the contents of deb_cols and incr and we will compute and set the
      // input columns by ourselves
      const unsigned int input_size =
          std::remove_reference<NewTranslator>::type::input_size;
      unsigned int input_cols[input_size];
      unsigned int incr_cols[input_size];
      deb_cols.toArray(input_cols);
      incr.toArray(incr_cols);

      // now insert the translators
      for (unsigned int i = 0; i < nb_times; ++i) {
        // create the translator
        Translator *new_translator = new NewTranslator(translator);
        try {
          __translators.push_back(new_translator);
        } catch (...) {
          delete new_translator;
          __output_row.row().resize(__output_size);
          throw;
        }

        // assign its output columns and rowFilter
        new_translator->setOutputRow(__output_row);
        new_translator->setOutputCols(__output_size);
        __output_size += std::remove_reference<NewTranslator>::type::output_size;

        // assign the input col
        unsigned int *inputs =
            const_cast<unsigned int *>(new_translator->inputCols());
        std::memcpy(inputs, input_cols, input_size * sizeof(unsigned int));
        for (unsigned int j = 0; j < input_size; ++j) {
          inputs[j] = input_cols[j];
          input_cols[j] += incr_cols[j];
        }
      }

      __output_row.row().resize(__output_size);
    }

    /// inserts new translators at the end of the vector
    template <typename Translator>
    void DBRowTranslatorSetDynamic<Translator>::insertTranslator(
        unsigned int deb_col, unsigned int nb_times, unsigned int increment) {
      for (unsigned int i = 0; i < nb_times; ++i) {
        // create the translator
        Translator *new_translator = new Translator;
        try {
          __translators.push_back(new_translator);
        } catch (...) {
          delete new_translator;
          __output_row.row().resize(__output_size);
          throw;
        }

        // assign its output columns and rowFilter
        new_translator->setOutputRow(__output_row);
        new_translator->setOutputCols(__output_size);
        __output_size += std::remove_reference<Translator>::type::output_size;

        // assign the input col
        unsigned int *inputs =
            const_cast<unsigned int *>(new_translator->inputCols());
        for (unsigned int j = 0;
             j < std::remove_reference<Translator>::type::input_size; ++j) {
          inputs[j] = deb_col + j;
        }
        deb_col += increment;
      }

      __output_row.row().resize(__output_size);
    }

    /// execute all the translations on the current database row
    template <typename Translator>
    ALWAYS_INLINE void DBRowTranslatorSetDynamic<Translator>::translate() {
      for (auto translator : __translators) {
        translator->translate();
      }
    }

    /// initialize the cell filters by parsing once the database
    template <typename Translator>
    ALWAYS_INLINE void DBRowTranslatorSetDynamic<Translator>::initialize() {
      for (auto translator : __translators) {
        translator->initialize();
      }
    }

    /// initialize the cell filters by parsing once the database
    template <typename Translator>
    INLINE void DBRowTranslatorSetDynamic<Translator>::postInitialize() {
      for (auto translator : __translators) {
        translator->postInitialize();
      }
    }

    /// indicates whether one of the translators needs an initial parsing
    /// of the database to initialize itself
    template <typename Translator>
    INLINE bool DBRowTranslatorSetDynamic<Translator>::requiresInitialization() const
        noexcept {
      for (auto translator : __translators) {
        if (translator->requiresInitialization())
          return true;
      }
      return false;
    }

    /// push back the number of modalities of the variables of the output columns
    template <typename Translator>
    INLINE void DBRowTranslatorSetDynamic<Translator>::modalities(
        std::vector<unsigned int> &modals) const {
      for (auto translator : __translators) {
        translator->modalities(modals);
      }
    }

    /// sets the input row that shall be read by all the cell translators
    template <typename Translator>
    INLINE void
    DBRowTranslatorSetDynamic<Translator>::setInputRow(const DBRow &row) noexcept {
      for (auto translator : __translators) {
        translator->setInputRow(row);
      }
      __output_row.weight() = row.weight();
    }

    /// returns the current output row
    template <typename Translator>
    INLINE FilteredRow &DBRowTranslatorSetDynamic<Translator>::outputRow() noexcept {
      return __output_row;
    }

    /// returns the name of the jth value of the ith column
    template <typename Translator>
    std::string DBRowTranslatorSetDynamic<Translator>::translateBack(
        unsigned int col, unsigned int translated_val) const {
      const unsigned int size = __translators.size();
      if (size == 0) {
        GUM_ERROR(UndefinedElement, "the set of translators is empty, so it is "
                                    "not possible to translate back a value");
      }
      unsigned int i = 0, j = 0;
      for (; i < size && col >= j + __translators[i]->outputSize();
           j += __translators[i]->outputSize(), ++i) {
      }
      if (i >= size) {
        GUM_ERROR(UndefinedElement, "the set of translators does not contain "
                                    "the column to be translated back");
      }

      return __translators[i]->translateBack(col - j, translated_val);
    }

    /// returns the current input DBRow
    template <typename Translator>
    INLINE const DBRow &DBRowTranslatorSetDynamic<Translator>::inputRow() const {
      if (__translators.empty())
        GUM_ERROR(UndefinedElement,
                  "There are no translators in the "
                  "DBRowTranslatorSetDynamic, to there is no input row");
      return __translators[0]->inputRow();
    }

    /// returns the size of the input as used by the cell translators
    template <typename Translator>
    INLINE unsigned int DBRowTranslatorSetDynamic<Translator>::inputSize() const
        noexcept {
      unsigned int size = 0;
      for (auto translator : __translators) {
        size += translator->inputSize();
      }
      return size;
    }

    /// returns the size of the output of the cell translators
    template <typename Translator>
    INLINE unsigned int DBRowTranslatorSetDynamic<Translator>::outputSize() const
        noexcept {
      return __output_size;
    }

    /// returns the ith translator
    template <typename Translator>
    INLINE Translator &DBRowTranslatorSetDynamic<Translator>::
    operator[](unsigned int i) {
      if (__translators.size() <= i) {
        GUM_ERROR(NotFound, "the translator cannot be found");
      }
      return *(__translators[i]);
    }

    /// returns the ith translator
    template <typename Translator>
    INLINE const Translator &DBRowTranslatorSetDynamic<Translator>::
    operator[](unsigned int i) const {
      if (__translators.size() <= i) {
        GUM_ERROR(NotFound, "the translator cannot be found");
      }
      return *(__translators[i]);
    }

    /// returns the number of translators stored into the set
    template <typename Translator>
    INLINE unsigned int DBRowTranslatorSetDynamic<Translator>::nbTranslators() const
        noexcept {
      return __translators.size();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
