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
 * @brief Template implementation of cell translators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <int Nb_inputs, int Nb_outputs>
    INLINE DBCellTranslator<Nb_inputs, Nb_outputs>::DBCellTranslator() noexcept {
      GUM_CONSTRUCTOR(DBCellTranslator);
    }

    /// copy constructor
    template <int Nb_inputs, int Nb_outputs>
    INLINE DBCellTranslator<Nb_inputs, Nb_outputs>::DBCellTranslator(
        const DBCellTranslator<Nb_inputs, Nb_outputs> &translator) noexcept
        : _input_row(translator._input_row),
          _output_row(translator._output_row) {
      GUM_CONS_CPY(DBCellTranslator);

      memcpy(_input_cols, translator._input_cols, Nb_inputs * sizeof(unsigned int));
      memcpy(_output_cols, translator._output_cols,
             Nb_outputs * sizeof(unsigned int));
    }

    /// move constructor
    template <int Nb_inputs, int Nb_outputs>
    INLINE DBCellTranslator<Nb_inputs, Nb_outputs>::DBCellTranslator(
        DBCellTranslator<Nb_inputs, Nb_outputs> &&translator) noexcept
        : _input_row(translator._input_row),
          _output_row(translator._output_row) {
      GUM_CONS_MOV(DBCellTranslator);

      memcpy(_input_cols, translator._input_cols, Nb_inputs * sizeof(unsigned int));
      memcpy(_output_cols, translator._output_cols,
             Nb_outputs * sizeof(unsigned int));
    }

    /// destructor
    template <int Nb_inputs, int Nb_outputs>
    INLINE DBCellTranslator<Nb_inputs, Nb_outputs>::~DBCellTranslator() noexcept {
      GUM_DESTRUCTOR(DBCellTranslator);
    }

    /// copy operator
    template <int Nb_inputs, int Nb_outputs>
    INLINE DBCellTranslator<Nb_inputs, Nb_outputs> &
        DBCellTranslator<Nb_inputs, Nb_outputs>::
        operator=(const DBCellTranslator<Nb_inputs, Nb_outputs> &translator) {
      if (this != &translator) {
        _input_row = translator._input_row;
        _output_row = translator._output_row;
        memcpy(_input_cols, translator._input_cols,
               Nb_inputs * sizeof(unsigned int));
        memcpy(_output_cols, translator._output_cols,
               Nb_outputs * sizeof(unsigned int));
      }
      return *this;
    }

    /// move operator
    template <int Nb_inputs, int Nb_outputs>
    INLINE DBCellTranslator<Nb_inputs, Nb_outputs> &
        DBCellTranslator<Nb_inputs, Nb_outputs>::
        operator=(DBCellTranslator<Nb_inputs, Nb_outputs> &&translator) {
      if (this != &translator) {
        _input_row = translator._input_row;
        _output_row = translator._output_row;
        memcpy(_input_cols, translator._input_cols,
               Nb_inputs * sizeof(unsigned int));
        memcpy(_output_cols, translator._output_cols,
               Nb_outputs * sizeof(unsigned int));
      }
      return *this;
    }

    /// sets a new DBRow to translate
    template <int Nb_inputs, int Nb_outputs>
    INLINE void
    DBCellTranslator<Nb_inputs, Nb_outputs>::setInputRow(const DBRow &row) noexcept {
      _input_row = &row;
    }

    /// sets a new FilteredRow to which the translator will write its output
    template <int Nb_inputs, int Nb_outputs>
    INLINE void DBCellTranslator<Nb_inputs, Nb_outputs>::setOutputRow(
        FilteredRow &row) noexcept {
      _output_row = &row;
    }

    /// sets the input DBRow's columns read by the translator
    template <int Nb_inputs, int Nb_outputs>
    template <int Col1, int... OtherCols>
    INLINE void DBCellTranslator<Nb_inputs, Nb_outputs>::setInputCols(
        const Col<Col1, OtherCols...> &) noexcept {
      static_assert(sizeof...(OtherCols)+1 == Nb_inputs,
                    "you set a wrong number of translator's input columns");
      Col<Col1, OtherCols...>::toArray(_input_cols);
    }

    /// sets the output FilteredRow's columns written by the translator
    template <int Nb_inputs, int Nb_outputs>
    INLINE void DBCellTranslator<Nb_inputs, Nb_outputs>::setOutputCols(
        unsigned int start) noexcept {
      for (unsigned int i = 0; i < Nb_outputs; ++i) {
        _output_cols[i] = start + i;
      }
    }

    /// returns the current input DBRow
    template <int Nb_inputs, int Nb_outputs>
    INLINE const DBRow &DBCellTranslator<Nb_inputs, Nb_outputs>::inputRow() const
        noexcept {
      return *_input_row;
    }

    /// returns the current output FilteredRow
    template <int Nb_inputs, int Nb_outputs>
    INLINE FilteredRow &
    DBCellTranslator<Nb_inputs, Nb_outputs>::outputFilteredRow() noexcept {
      return *_output_row;
    }

    /// returns the row of unsigned int of the current output FilteredRow
    template <int Nb_inputs, int Nb_outputs>
    INLINE std::vector<unsigned int> &
    DBCellTranslator<Nb_inputs, Nb_outputs>::outputRow() noexcept {
      return _output_row->row();
    }

    /// returns the set of input DBRow's columns used by the translator
    template <int Nb_inputs, int Nb_outputs>
    INLINE const unsigned int *
    DBCellTranslator<Nb_inputs, Nb_outputs>::inputCols() const noexcept {
      return _input_cols;
    }

    /// returns the set of output FilteredRow's columns written by the translator
    template <int Nb_inputs, int Nb_outputs>
    INLINE const unsigned int *
    DBCellTranslator<Nb_inputs, Nb_outputs>::outputCols() const noexcept {
      return _output_cols;
    }

    /// returns the DBCell read at the ith input column of translator
    template <int Nb_inputs, int Nb_outputs>
    INLINE const DBCell &
    DBCellTranslator<Nb_inputs, Nb_outputs>::in(unsigned int i) const noexcept {
      return _input_row->operator[](_input_cols[i]);
    }

    /// returns the FilteredRow cell corresponding to the ith output column
    template <int Nb_inputs, int Nb_outputs>
    INLINE unsigned int &
    DBCellTranslator<Nb_inputs, Nb_outputs>::out(unsigned int i) noexcept {
      return _output_row->row()[_output_cols[i]];
    }

    /// returns the size of the input for this cell translator
    template <int Nb_inputs, int Nb_outputs>
    INLINE unsigned int DBCellTranslator<Nb_inputs, Nb_outputs>::inputSize() const
        noexcept {
      return Nb_inputs;
    }

    /// returns the size of the output for this cell translator
    template <int Nb_inputs, int Nb_outputs>
    INLINE unsigned int DBCellTranslator<Nb_inputs, Nb_outputs>::outputSize() const
        noexcept {
      return Nb_outputs;
    }

    /// for friendly displaying the content of translators
    template <int Nb_inputs, int Nb_outputs>
    std::ostream &operator<<(std::ostream &stream,
                             const DBCellTranslator<Nb_inputs, Nb_outputs> &tr) {
      stream << tr.toString();
      return stream;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
