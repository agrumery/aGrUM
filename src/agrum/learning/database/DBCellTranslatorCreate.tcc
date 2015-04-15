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
 * @brief Template implementation of helper class Create
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// creates a wrapper to execute a translator on a given set of columns
    template <class Translator, typename Cols>
    INLINE CreateOnce<Translator, Cols>::CreateOnce(const Translator &translator,
                                                    const Cols &s)
        : __translator(translator) {
      // check that the number of columns in "s" matches what the translator
      // is waiting for
      static_assert(Translator::input_size == Cols::size,
                    "the number of columns is not compatible with the translator");
      static_assert(std::is_base_of<BaseDBCellTranslator, Translator>::value,
                    "Error: you should pass only CellTranslators to Create<>");

      GUM_CONSTRUCTOR(CreateOnce);

      __translator.setInputCols(s);
    }

    /// copy constructor
    template <class Translator, typename Cols>
    INLINE CreateOnce<Translator, Cols>::CreateOnce(
        const CreateOnce<Translator, Cols> &call)
        : __translator(call.__translator) {
      GUM_CONS_CPY(CreateOnce);
    }

    /// move constructor
    template <class Translator, typename Cols>
    INLINE
    CreateOnce<Translator, Cols>::CreateOnce(CreateOnce<Translator, Cols> &&call)
        : __translator(std::move(call.__translator)) {
      GUM_CONS_MOV(CreateOnce);
    }

    /// destructor
    template <class Translator, typename Cols>
    INLINE CreateOnce<Translator, Cols>::~CreateOnce() noexcept {
      GUM_DESTRUCTOR(CreateOnce);
    }

    /// copy operator
    template <class Translator, typename Cols>
    INLINE CreateOnce<Translator, Cols> &CreateOnce<Translator, Cols>::
    operator=(const CreateOnce<Translator, Cols> &call) {
      if (this != &call) {
        __translator = call.__translator;
      }
      return *this;
    }

    /// move operator
    template <class Translator, typename Cols>
    INLINE CreateOnce<Translator, Cols> &CreateOnce<Translator, Cols>::
    operator=(CreateOnce<Translator, Cols> &&call) {
      if (this != &call) {
        __translator = std::move(call.__translator);
      }
      return *this;
    }

    /// sets the row that will be read by the translator
    template <class Translator, typename Cols>
    INLINE void
    CreateOnce<Translator, Cols>::setInputRow(const DBRow &row) noexcept {
      __translator.setInputRow(row);
    }

    /// sets the row to which the output will be written by the translator
    template <class Translator, typename Cols>
    INLINE void
    CreateOnce<Translator, Cols>::setOutputRow(FilteredRow &row) noexcept {
      __translator.setOutputRow(row);
    }

    /** @brief sets the columns of the output vector which will be written
     * by the translator */
    template <class Translator, typename Cols>
    INLINE void
    CreateOnce<Translator, Cols>::setOutputCols(unsigned int first_col) noexcept {
      __translator.setOutputCols(first_col);
    }

    /// returns the translator that does all the job
    template <class Translator, typename Cols>
    INLINE Translator &CreateOnce<Translator, Cols>::getTranslator() noexcept {
      return __translator;
    }

    /// returns the translator that does all the job
    template <class Translator, typename Cols>
    INLINE const Translator &CreateOnce<Translator, Cols>::getTranslator() const
        noexcept {
      return __translator;
    }

    /// execute a translation of the input row
    template <class Translator, typename Cols>
    ALWAYS_INLINE void CreateOnce<Translator, Cols>::translate() {
      __translator.translate();
    }

    /// back-translate a given output (i.e., returns its input)
    template <class Translator, typename Cols>
    INLINE std::string
    CreateOnce<Translator, Cols>::translateBack(unsigned int col,
                                                unsigned int translated_val) const {
      return __translator.translateBack(col, translated_val);
    }

    /// initialize the cell filters by parsing once the database
    template <class Translator, typename Cols>
    ALWAYS_INLINE void CreateOnce<Translator, Cols>::initialize() {
      __translator.initialize();
    }

    /// perform a post initialization after the database parsing
    template <class Translator, typename Cols>
    INLINE void CreateOnce<Translator, Cols>::postInitialize() {
      __translator.postInitialize();
    }

    /** @brief indicates whether the translator needs an initial parsing of the
     * database to initialize itself */
    template <class Translator, typename Cols>
    INLINE bool CreateOnce<Translator, Cols>::requiresInitialization() const
        noexcept {
      return __translator.requiresInitialization();
    }

    /// push back the number of modalities of the variables of the output columns
    template <class Translator, typename Cols>
    INLINE void CreateOnce<Translator, Cols>::modalities(
        std::vector<unsigned int> &modals) const {
      __translator.modalities(modals);
    }

    /// returns the current input DBRow
    template <class Translator, typename Cols>
    INLINE const DBRow &CreateOnce<Translator, Cols>::inputRow() const noexcept {
      return __translator.inputRow();
    }

    /// returns the current output FilteredRow
    template <class Translator, typename Cols>
    INLINE FilteredRow &CreateOnce<Translator, Cols>::outputFilteredRow() noexcept {
      return __translator.outputFilteredRow();
    }

    /// returns the row of unsigned int of the current output FilteredRow
    template <class Translator, typename Cols>
    INLINE std::vector<unsigned int> &
    CreateOnce<Translator, Cols>::outputRow() noexcept {
      return __translator.outputRow();
    }

    /// returns the size of the input for this cell translator
    template <class Translator, typename Cols>
    INLINE unsigned int CreateOnce<Translator, Cols>::inputSize() const noexcept {
      return __translator.inputSize();
    }

    /// returns the size of the output for this cell translator
    template <class Translator, typename Cols>
    INLINE unsigned int CreateOnce<Translator, Cols>::outputSize() const noexcept {
      return __translator.outputSize();
    }

    // ===========================================================================

    /// default constructor
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE Create<Translator, Cols, nb_times, ColsIncr>::Create() {
      GUM_CONSTRUCTOR(Create);
    }

    /// copy constructor
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE Create<Translator, Cols, nb_times, ColsIncr>::Create(
        const Create<Translator, Cols, nb_times, ColsIncr> &call)
        : CurrentTranslator(call), NextTranslators(call) {
      GUM_CONS_CPY(Create);
    }

    /// move constructor
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE Create<Translator, Cols, nb_times, ColsIncr>::Create(
        Create<Translator, Cols, nb_times, ColsIncr> &&call)
        : CurrentTranslator(std::move(call)), NextTranslators(std::move(call)) {
      GUM_CONS_MOV(Create);
    }

    /// destructor
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE Create<Translator, Cols, nb_times, ColsIncr>::~Create() noexcept {
      GUM_DESTRUCTOR(Create);
    }

    /// copy operator
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE Create<Translator, Cols, nb_times, ColsIncr> &
        Create<Translator, Cols, nb_times, ColsIncr>::
        operator=(const Create<Translator, Cols, nb_times, ColsIncr> &from) {
      if (this != &from) {
        CurrentTranslator::operator=(from);
        NextTranslators::operator=(from);
      }
      return *this;
    }

    /// move operator
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE Create<Translator, Cols, nb_times, ColsIncr> &
        Create<Translator, Cols, nb_times, ColsIncr>::
        operator=(Create<Translator, Cols, nb_times, ColsIncr> &&from) {
      if (this != &from) {
        CurrentTranslator::operator=(std::move(from));
        NextTranslators::operator=(std::move(from));
      }
      return *this;
    }

    /// sets the output columns written by all the applications of the translator
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE void Create<Translator, Cols, nb_times, ColsIncr>::setOutputCols(
        unsigned int first_col) noexcept {
      CurrentTranslator::setOutputCols(first_col);
      NextTranslators::setOutputCols(first_col + Translator::output_size);
    }

    /// sets the input row which will be read by the translator
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE void Create<Translator, Cols, nb_times, ColsIncr>::setInputRow(
        const DBRow &row) noexcept {
      CurrentTranslator::setInputRow(row);
      NextTranslators::setInputRow(row);
    }

    /// sets the row to which the translator will write its output
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE void Create<Translator, Cols, nb_times, ColsIncr>::setOutputRow(
        FilteredRow &row) noexcept {
      CurrentTranslator::setOutputRow(row);
      NextTranslators::setOutputRow(row);
    }

    /// returns the current input DBRow
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE const DBRow &
    Create<Translator, Cols, nb_times, ColsIncr>::inputRow() const noexcept {
      return CurrentTranslator::inputRow();
    }

    /// returns the current output FilteredRow
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE FilteredRow &
    Create<Translator, Cols, nb_times, ColsIncr>::outputFilteredRow() noexcept {
      return CurrentTranslator::outputFilteredRow();
    }

    /// returns the row of unsigned int of the current output FilteredRow
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE std::vector<unsigned int> &
    Create<Translator, Cols, nb_times, ColsIncr>::outputRow() noexcept {
      return CurrentTranslator::outputRow();
    }

    /// apply the translator
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    ALWAYS_INLINE void Create<Translator, Cols, nb_times, ColsIncr>::translate() {
      CurrentTranslator::translate();
      NextTranslators::translate();
    }

    /// back-translate a given output (i.e., returns its input)
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE std::string Create<Translator, Cols, nb_times, ColsIncr>::translateBack(
        unsigned int col, unsigned int translated_val) const {
      if (col < Translator::output_size) {
        return CurrentTranslator::translateBack(col, translated_val);
      } else {
        return NextTranslators::translateBack(col - Translator::output_size,
                                              translated_val);
      }
    }

    /// initialize the cell filters by parsing once the database
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    ALWAYS_INLINE void Create<Translator, Cols, nb_times, ColsIncr>::initialize() {
      CurrentTranslator::initialize();
      NextTranslators::initialize();
    }

    /// perform a post initialization after the database parsing
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE void Create<Translator, Cols, nb_times, ColsIncr>::postInitialize() {
      CurrentTranslator::postInitialize();
      NextTranslators::postInitialize();
    }

    /** @brief indicates whether the translator needs an initial parsing of the
     * database to initialize itself */
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE bool
    Create<Translator, Cols, nb_times, ColsIncr>::requiresInitialization() const
        noexcept {
      return CurrentTranslator::requiresInitialization() ||
             NextTranslators::requiresInitialization();
    }

    /// push back the number of modalities of the variables of the output columns
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE void Create<Translator, Cols, nb_times, ColsIncr>::modalities(
        std::vector<unsigned int> &modals) const {
      CurrentTranslator::modalities(modals);
      NextTranslators::modalities(modals);
    }

    /// returns the size of the input for the cell translators
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE unsigned int
    Create<Translator, Cols, nb_times, ColsIncr>::inputSize() const noexcept {
      return nb_times * CurrentTranslator::inputSize();
    }

    /// returns the size of the output for the cell translators
    template <typename Translator, typename Cols, int nb_times, typename ColsIncr>
    INLINE unsigned int
    Create<Translator, Cols, nb_times, ColsIncr>::outputSize() const noexcept {
      return nb_times * CurrentTranslator::outputSize();
    }

  } /* namespace learning */

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
