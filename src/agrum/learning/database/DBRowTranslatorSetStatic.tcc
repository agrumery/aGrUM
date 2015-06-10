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
 * @brief Template implementation of DBRowTranslatorSetStatics
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::
        BasicDBRowTranslatorSetStatic(const Translator &first_translator,
                                      const OtherTranslators &... next_translators)
        : NextTranslators(next_translators...), __translator(first_translator) {
      GUM_CONSTRUCTOR(BasicDBRowTranslatorSetStatic);
      __translator.setOutputCols(Idx);
    }

    /// copy constructor
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::
        BasicDBRowTranslatorSetStatic(const BasicDBRowTranslatorSetStatic<
            Idx, Translator, OtherTranslators...> &from)
        : NextTranslators(from), __translator(from.__translator) {
      GUM_CONS_CPY(BasicDBRowTranslatorSetStatic);
      __translator.setOutputCols(Idx);
    }

    /// move constructor
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::
        BasicDBRowTranslatorSetStatic(BasicDBRowTranslatorSetStatic<
            Idx, Translator, OtherTranslators...> &&from)
        : NextTranslators(std::move(from)),
          __translator(std::move(from.__translator)) {
      GUM_CONS_MOV(BasicDBRowTranslatorSetStatic);
      __translator.setOutputCols(Idx);
    }

    /// destructor
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE BasicDBRowTranslatorSetStatic<
        Idx, Translator,
        OtherTranslators...>::~BasicDBRowTranslatorSetStatic() noexcept {
      GUM_DESTRUCTOR(BasicDBRowTranslatorSetStatic);
    }

    /// copy operator
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...> &
        BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::
        operator=(const BasicDBRowTranslatorSetStatic<Idx, Translator,
                                                      OtherTranslators...> &from) {
      if (this != &from) {
        NextTranslators::operator=(from);
        __translator = from.__translator;
        __translator.setOutputCols(Idx);
      }
      return *this;
    }

    /// move operator
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...> &
        BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::
        operator=(BasicDBRowTranslatorSetStatic<Idx, Translator,
                                                OtherTranslators...> &&from) {
      if (this != &from) {
        NextTranslators::operator=(std::move(from));
        __translator = std::move(from.__translator);
        __translator.setOutputCols(Idx);
      }
      return *this;
    }

    /// sets the input row to filter
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE void
    BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::setInputRow(
        const DBRow &row) noexcept {
      __translator.setInputRow(row);
      NextTranslators::setInputRow(row);
    }

    /// sets the output rows of the translators
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE void BasicDBRowTranslatorSetStatic<
        Idx, Translator,
        OtherTranslators...>::setOutputRow(FilteredRow &output_row) noexcept {
      __translator.setOutputRow(output_row);
      NextTranslators::setOutputRow(output_row);
    }

    /// returns the current input DBRow
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE const DBRow &
    BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::inputRow()
        const {
      return __translator.inputRow();
    }

    /// returns the current output row
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE FilteredRow &BasicDBRowTranslatorSetStatic<
        Idx, Translator, OtherTranslators...>::outputRow() {
      return __translator.outputRow();
    }

    /// perform the translation of a database row
    template <int Idx, typename Translator, typename... OtherTranslators>
    ALWAYS_INLINE void BasicDBRowTranslatorSetStatic<
        Idx, Translator, OtherTranslators...>::translate() {
      __translator.translate();
      NextTranslators::translate();
    }

    /// returns the name of the jth value of the ith column
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE std::string BasicDBRowTranslatorSetStatic<
        Idx, Translator,
        OtherTranslators...>::translateBack(unsigned int col,
                                            unsigned int translated_val) const {
      if (col < Translator::output_size) {
        return __translator.translateBack(col, translated_val);
      } else {
        return NextTranslators::translateBack(col - Translator::output_size,
                                              translated_val);
      }
    }

    /// initialize the cell filters by parsing once the database
    template <int Idx, typename Translator, typename... OtherTranslators>
    ALWAYS_INLINE void BasicDBRowTranslatorSetStatic<
        Idx, Translator, OtherTranslators...>::initialize() {
      __translator.initialize();
      NextTranslators::initialize();
    }

    /// perform a post initialization after the database parsing
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE void BasicDBRowTranslatorSetStatic<
        Idx, Translator, OtherTranslators...>::postInitialize() {
      __translator.postInitialize();
      NextTranslators::postInitialize();
    }

    /** @brief indicates whether one of the translators needs an initial parsing
     * of the database to initialize itself */
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE bool BasicDBRowTranslatorSetStatic<
        Idx, Translator, OtherTranslators...>::requiresInitialization() const
        noexcept {
      return __translator.requiresInitialization() ||
             NextTranslators::requiresInitialization();
    }

    /// push back the number of modalities of the variables of the output columns
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE void
    BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::modalities(
        std::vector<unsigned int> &modals) const {
      __translator.modalities(modals);
      NextTranslators::modalities(modals);
    }

    /// returns the size of the input as used by the cell translators
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE unsigned int
    BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::inputSize()
        const noexcept {
      return __translator.inputSize() + NextTranslators::inputSize();
    }

    /// returns the size of the output of the cell translators
    template <int Idx, typename Translator, typename... OtherTranslators>
    INLINE unsigned int
    BasicDBRowTranslatorSetStatic<Idx, Translator, OtherTranslators...>::outputSize()
        const noexcept {
      return output_size;
    }

    // ===========================================================================

    /// default constructor
    template <typename... Translators>
    INLINE DBRowTranslatorSetStatic<Translators...>::DBRowTranslatorSetStatic(
        const Translators &... translators)
        : TranslatorSetStatic(translators...) {
      GUM_CONSTRUCTOR(DBRowTranslatorSetStatic);

      // resize appropriately the output vector w.r.t. the applied translators
      __output_row.row().resize(TranslatorSetStatic::output_size);

      // assign the class' output row to all the CellTranslators
      TranslatorSetStatic::setOutputRow(__output_row);
    }

    /// copy constructor
    template <typename... Translators>
    INLINE DBRowTranslatorSetStatic<Translators...>::DBRowTranslatorSetStatic(
        const DBRowTranslatorSetStatic<Translators...> &from)
        : TranslatorSetStatic(from) {
      GUM_CONS_CPY(DBRowTranslatorSetStatic);

      __output_row.row().resize(TranslatorSetStatic::output_size);
      TranslatorSetStatic::setOutputRow(__output_row);
    }

    /// move constructor
    template <typename... Translators>
    INLINE DBRowTranslatorSetStatic<Translators...>::DBRowTranslatorSetStatic(
        DBRowTranslatorSetStatic<Translators...> &&from)
        : TranslatorSetStatic(std::move(from)),
          __output_row(std::move(from.__output_row)) {
      GUM_CONS_MOV(DBRowTranslatorSetStatic);
      TranslatorSetStatic::setOutputRow(__output_row);
    }

    /// destructor
    template <typename... Translators>
    INLINE DBRowTranslatorSetStatic<
        Translators...>::~DBRowTranslatorSetStatic() noexcept {
      GUM_DESTRUCTOR(DBRowTranslatorSetStatic);
    }

    /// copy operator
    template <typename... Translators>
    INLINE DBRowTranslatorSetStatic<Translators...> &
        DBRowTranslatorSetStatic<Translators...>::
        operator=(const DBRowTranslatorSetStatic<Translators...> &from) {
      if (this != &from) {
        TranslatorSetStatic::operator=(from);
        __output_row.row().resize(TranslatorSetStatic::output_size);
        TranslatorSetStatic::setOutputRow(__output_row);
      }
      return *this;
    }

    /// move operator
    template <typename... Translators>
    INLINE DBRowTranslatorSetStatic<Translators...> &
        DBRowTranslatorSetStatic<Translators...>::
        operator=(DBRowTranslatorSetStatic<Translators...> &&from) {
      if (this != &from) {
        TranslatorSetStatic::operator=(std::move(from));
        __output_row = std::move(from.__output_row);
        TranslatorSetStatic::setOutputRow(__output_row);
      }
      return *this;
    }

    /// execute all the translations on the current database row
    template <typename... Translators>
    ALWAYS_INLINE void DBRowTranslatorSetStatic<Translators...>::translate() {
      TranslatorSetStatic::translate();
    }

    /// back-translate a given output (i.e., returns its input)
    template <typename... Translators>
    ALWAYS_INLINE std::string
    DBRowTranslatorSetStatic<Translators...>::translateBack(
        unsigned int col, unsigned int translated_val) const {
      return TranslatorSetStatic::translateBack(col, translated_val);
    }

    /// initialize the cell filters by parsing once the database
    template <typename... Translators>
    ALWAYS_INLINE void DBRowTranslatorSetStatic<Translators...>::initialize() {
      TranslatorSetStatic::initialize();
    }

    /// initialize the cell filters by parsing once the database
    template <typename... Translators>
    INLINE void DBRowTranslatorSetStatic<Translators...>::postInitialize() {
      TranslatorSetStatic::postInitialize();
    }

    /** @brief indicates whether one of the translators needs an initial parsing
     * of the database to initialize itself */
    template <typename... Translators>
    INLINE bool
    DBRowTranslatorSetStatic<Translators...>::requiresInitialization() const
        noexcept {
      return TranslatorSetStatic::requiresInitialization();
    }

    /// push back the number of modalities of the variables of the output columns
    template <typename... Translators>
    INLINE void DBRowTranslatorSetStatic<Translators...>::modalities(
        std::vector<unsigned int> &modals) const {
      return TranslatorSetStatic::modalities(modals);
    }

    /// sets the input row that shall be read by all the cell translators
    template <typename... Translators>
    INLINE void DBRowTranslatorSetStatic<Translators...>::setInputRow(
        const DBRow &row) noexcept {
      TranslatorSetStatic::setInputRow(row);
      __output_row.weight() = row.weight();
    }

    /// returns the current output row
    template <typename... Translators>
    INLINE FilteredRow &
    DBRowTranslatorSetStatic<Translators...>::outputRow() noexcept {
      return __output_row;
    }

    /// returns the current input DBRow
    template <typename... Translators>
    INLINE const DBRow &DBRowTranslatorSetStatic<Translators...>::inputRow() const {
      return TranslatorSetStatic::inputRow();
    }

    /// returns the size of the input as used by the cell translators
    template <typename... Translators>
    INLINE unsigned int DBRowTranslatorSetStatic<Translators...>::inputSize() const
        noexcept {
      return TranslatorSetStatic::inputSize();
    }

    /// returns the size of the output of the cell translators
    template <typename... Translators>
    INLINE unsigned int DBRowTranslatorSetStatic<Translators...>::outputSize() const
        noexcept {
      return const_cast<FilteredRow &>(__output_row).row().size();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
