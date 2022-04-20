/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Class for packing sets of generators
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/tools/database/DBRowGeneratorSet.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/DBRowGeneratorSet_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy constructor
    DBRowGeneratorSet::DBRowGeneratorSet(const DBRowGeneratorSet& from) :
        _generators_(from._nb_generators_, nullptr), _nb_generators_(from._nb_generators_),
        _setInputRow_performed_(from._nb_generators_, 0) {
      // create the generators
      for (std::size_t i = std::size_t(0); i < _nb_generators_; ++i)
        _generators_[i] = from._generators_[i]->clone();

      GUM_CONS_CPY(DBRowGeneratorSet);
    }


    /// virtual copy constructor
    DBRowGeneratorSet* DBRowGeneratorSet::clone() const { return new DBRowGeneratorSet(*this); }


    /// removes all the generators
    void DBRowGeneratorSet::clear() {
      // delete all the generators
      for (auto gen: _generators_) {
        delete gen;
      }

      // clear all the internal fields
      _generators_.clear();
      _nb_generators_ = std::size_t(0);
      _output_row_    = nullptr;
      _setInputRow_performed_.clear();
    }


    /// destructor
    DBRowGeneratorSet::~DBRowGeneratorSet() {
      GUM_DESTRUCTOR(DBRowGeneratorSet);
      clear();
    }


    /// copy operator
    DBRowGeneratorSet& DBRowGeneratorSet::operator=(const DBRowGeneratorSet& from) {
      if (this != &from) {
        // produce the new generators
        std::vector< DBRowGenerator* > new_generators(from._nb_generators_, nullptr);
        for (std::size_t i = std::size_t(0); i < from._nb_generators_; ++i) {
          try {
            new_generators[i] = from._generators_[i]->clone();
          } catch (...) {
            for (std::size_t j = std::size_t(0); j < i; ++j) {
              delete new_generators[j];
            }
            throw;
          }
        }

        // create the setInputDBrow_performed vector
        std::vector< int > setInputDBrow_performed(from._nb_generators_, 0);

        // remove the old generators and copy the new ones
        clear();
        _generators_            = std::move(new_generators);
        _nb_generators_         = from._nb_generators_;
        _output_row_            = nullptr;
        _setInputRow_performed_ = std::move(setInputDBrow_performed);
      }

      return *this;
    }


    /// move operator
    DBRowGeneratorSet& DBRowGeneratorSet::operator=(DBRowGeneratorSet&& from) {
      if (this != &from) {
        // remove the old generators and move the new ones
        clear();
        _generators_            = std::move(from._generators_);
        _nb_generators_         = from._nb_generators_;
        _output_row_            = from._output_row_;
        _setInputRow_performed_ = std::move(from._setInputRow_performed_);
      }
      return *this;
    }


    // try to produce a new row
    bool DBRowGeneratorSet::_produceNextRow_(const DBRow< DBTranslatedValue >* input_row,
                                             std::size_t                       i) {
      // the generation of output rows can be viewed as the traversal of a
      // tree: each node of the tree correspond to the input row received by
      // a generator. So the root node is the row passed in argument to
      // the setInputDBrow() Method. From these input rows, generators produce
      // through their generate() method new output rows, which correspond to
      // the input rows of the next level of the tree. If we traverse this tree
      // in terms of generators rather than in terms of input rows, we should
      // call once Method setInputDBrow() in order to update the generators
      // data structures, and then, to call method generate() to create new
      // output rows. When some generators are unable to produce output rows,
      // we just need to backtrack in the tree

      // for the ith generator, we set the new input DBRow passed in
      // argument of setInputDBRow. We ask it to generate a new output row.
      // If this can be done, the new output row is passed as a new input DBRow
      // for the next generator, and so on. If a generator cannot produce any
      // output row, we ask its predecessors to generate new rows (backtrack),
      // until all the generators have been able to generate at least one output
      // row (or no output row can be produced from input_row).
      const DBRow< DBTranslatedValue >* row = input_row;
      while (i != _nb_generators_) {
        auto generator = _generators_[i];

        // if we did not pass any row yet to the ith generator, do it
        // else use method generate() to generate a new output row
        if (_setInputRow_performed_[i] == 0) {
          // pass the current row
          const bool has_rows = generator->setInputRow(*row);

          // if the generator could not create output rows, try to backtrack
          if (!has_rows) {
            if (i > std::size_t(0)) {
              --i;
              continue;
            } else {
              // here we were unable to generate output rows
              _output_row_ = nullptr;
              return false;
            }
          } else {
            // here, the generator is able to generate output rows
            // so, generate the first one
            row                        = &(generator->generate());
            _setInputRow_performed_[i] = 1;

            // pass to the next generator
            ++i;
          }
        } else {
          // here, the generator has already performed its setInputDBRow call
          // so we should ask it to generate a new row. If it cannot produce
          // any more row, try to backtrack
          if (generator->hasRows()) {
            // get the new row
            row = &(generator->generate());

            // pass to the next generator
            ++i;
          } else {
            // indicate that the next time we use this generator, we will have
            // to use method setInputDBRow and backtrack
            _setInputRow_performed_[i] = 0;
            if (i > std::size_t(0)) {
              --i;
              continue;
            } else {
              // here we were unable to generate output rows
              _output_row_ = nullptr;
              return false;
            }
          }
        }
      }

      // here row contains a row generated on a leaf of the row generation tree
      // we should keep it when the user will ask for the next row to generate
      _output_row_ = row;
      return true;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
