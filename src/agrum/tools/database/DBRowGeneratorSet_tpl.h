/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


/** @file
 * @brief Template implementation of DBRowGeneratorSet
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE DBRowGeneratorSet< ALLOC >::DBRowGeneratorSet(
       const typename DBRowGeneratorSet< ALLOC >::allocator_type& alloc) :
        generators__(alloc),
        setInputRow_performed__(alloc) {
      GUM_CONSTRUCTOR(DBRowGeneratorSet);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBRowGeneratorSet< ALLOC >::DBRowGeneratorSet(
       const DBRowGeneratorSet< ALLOC >&                          from,
       const typename DBRowGeneratorSet< ALLOC >::allocator_type& alloc) :
        generators__(from.nb_generators__, nullptr, alloc),
        nb_generators__(from.nb_generators__),
        setInputRow_performed__(from.nb_generators__, 0, alloc) {
      // create the generators
      for (std::size_t i = std::size_t(0); i < nb_generators__; ++i)
        generators__[i] = from.generators__[i]->clone(alloc);

      GUM_CONS_CPY(DBRowGeneratorSet);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE DBRowGeneratorSet< ALLOC >::DBRowGeneratorSet(
       const DBRowGeneratorSet< ALLOC >& from) :
        DBRowGeneratorSet< ALLOC >(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE DBRowGeneratorSet< ALLOC >::DBRowGeneratorSet(
       DBRowGeneratorSet< ALLOC >&&                               from,
       const typename DBRowGeneratorSet< ALLOC >::allocator_type& alloc) :
        generators__(std::move(from.generators__), alloc),
        nb_generators__(from.nb_generators__), output_row__(from.output_row__),
        setInputRow_performed__(std::move(from.setInputRow_performed__), alloc) {
      GUM_CONS_MOV(DBRowGeneratorSet);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE DBRowGeneratorSet< ALLOC >::DBRowGeneratorSet(
       DBRowGeneratorSet< ALLOC >&& from) :
        DBRowGeneratorSet< ALLOC >(std::move(from), from.getAllocator()) {}

    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBRowGeneratorSet< ALLOC >* DBRowGeneratorSet< ALLOC >::clone(
       const typename DBRowGeneratorSet< ALLOC >::allocator_type& alloc) const {
      ALLOC< DBRowGeneratorSet< ALLOC > > allocator(alloc);
      DBRowGeneratorSet< ALLOC >*         generators = allocator.allocate(1);
      try {
        allocator.construct(generators, *this, alloc);
      } catch (...) {
        allocator.deallocate(generators, 1);
        throw;
      }
      return generators;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    INLINE DBRowGeneratorSet< ALLOC >* DBRowGeneratorSet< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// removes all the generators
    template < template < typename > class ALLOC >
    void DBRowGeneratorSet< ALLOC >::clear() {
      // delete all the generators
      ALLOC< DBRowGenerator< ALLOC > > allocator(this->getAllocator());
      for (auto gen: generators__) {
        allocator.destroy(gen);
        allocator.deallocate(gen, 1);
      }

      // clear all the internal fields
      generators__.clear();
      nb_generators__ = std::size_t(0);
      output_row__ = nullptr;
      setInputRow_performed__.clear();
    }


    /// destructor
    template < template < typename > class ALLOC >
    DBRowGeneratorSet< ALLOC >::~DBRowGeneratorSet() {
      GUM_DESTRUCTOR(DBRowGeneratorSet);
      clear();
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBRowGeneratorSet< ALLOC >& DBRowGeneratorSet< ALLOC >::operator=(
       const DBRowGeneratorSet< ALLOC >& from) {
      if (this != &from) {
        // produce the new generators
        ALLOC< DBRowGenerator< ALLOC > > allocator(this->getAllocator());
        std::vector< DBRowGenerator< ALLOC >*, ALLOC< DBRowGenerator< ALLOC >* > >
           new_generators(from.nb_generators__, nullptr, allocator);
        for (std::size_t i = std::size_t(0); i < from.nb_generators__; ++i) {
          try {
            new_generators[i] = from.generators__[i]->clone(allocator);
          } catch (...) {
            for (std::size_t j = std::size_t(0); j < i; ++j) {
              allocator.destroy(new_generators[j]);
              allocator.deallocate(new_generators[j], 1);
            }
            throw;
          }
        }

        // create the setInputDBrow_performed vector
        std::vector< int, ALLOC< int > > setInputDBrow_performed(
           from.nb_generators__, 0, getAllocator());

        // remove the old generators and copy the new ones
        clear();
        generators__ = std::move(new_generators);
        nb_generators__ = from.nb_generators__;
        output_row__ = nullptr;
        setInputRow_performed__ = std::move(setInputDBrow_performed);
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBRowGeneratorSet< ALLOC >&
       DBRowGeneratorSet< ALLOC >::operator=(DBRowGeneratorSet< ALLOC >&& from) {
      if (this != &from) {
        // remove the old generators and move the new ones
        clear();
        generators__ = std::move(from.generators__);
        nb_generators__ = from.nb_generators__;
        output_row__ = from.output_row__;
        setInputRow_performed__ = std::move(from.setInputRow_performed__);
      }
      return *this;
    }


    /// returns the ith generator
    template < template < typename > class ALLOC >
    INLINE DBRowGenerator< ALLOC >&
       DBRowGeneratorSet< ALLOC >::operator[](const std::size_t i) {
      return *(generators__[i]);
    }


    /// returns the ith generator
    template < template < typename > class ALLOC >
    INLINE const DBRowGenerator< ALLOC >&
       DBRowGeneratorSet< ALLOC >::operator[](const std::size_t i) const {
      return *(generators__[i]);
    }


    /// inserts a new generator at the end of the set
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class Generator >
    void DBRowGeneratorSet< ALLOC >::insertGenerator(
       const Generator< ALLOC >& generator) {
      // check that no output row generation is still active
      if (output_row__ != nullptr)
        GUM_ERROR(OperationNotAllowed,
                  "you cannot insert a new generator while a generation is "
                  "still being processed");

      ALLOC< DBRowGenerator< ALLOC > > allocator(this->getAllocator());
      generators__.push_back(generator.clone(allocator));

      try {
        setInputRow_performed__.push_back(0);
      } catch (...) {
        allocator.destroy(generators__.back());
        allocator.deallocate(generators__.back(), 1);
        throw;
      }

      ++nb_generators__;
      output_row__ = nullptr;
    }


    /// inserts a new generator at the end of the set
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class Generator >
    void DBRowGeneratorSet< ALLOC >::insertGenerator(
       const Generator< ALLOC >& generator, const std::size_t i) {
      // check that no output row generation is still active
      if (output_row__ != nullptr)
        GUM_ERROR(OperationNotAllowed,
                  "you cannot insert a new generator while a generation is "
                  "still being processed");

      ALLOC< DBRowGenerator< ALLOC > > allocator(this->getAllocator());
      generators__.insert(generators__.begin() + i, generator.clone(allocator));

      try {
        setInputRow_performed__.push_back(0);
      } catch (...) {
        allocator.destroy(*(generators__.begin() + i));
        allocator.deallocate(*(generators__.begin() + i));
        throw;
      }

      ++nb_generators__;
      output_row__ = nullptr;
    }


    /// returns the number of generators
    template < template < typename > class ALLOC >
    INLINE std::size_t DBRowGeneratorSet< ALLOC >::nbGenerators() const noexcept {
      return nb_generators__;
    }


    /// assign a new Bayes net to all the generators that depend on a BN
    template < template < typename > class ALLOC >
    template < typename GUM_SCALAR >
    void DBRowGeneratorSet< ALLOC >::setBayesNet(
       const BayesNet< GUM_SCALAR >& new_bn) {
      HashTable< DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >*,
                 const BayesNet< GUM_SCALAR >* >
         old_bns;

      for (auto xgen: generators__) {
        // check if the generator relies on a Bayes net
        DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >* gen = nullptr;
        try {
          gen = dynamic_cast< DBRowGeneratorWithBN< GUM_SCALAR, ALLOC >* >(xgen);
        } catch (std::bad_cast&) {}

        if (gen != nullptr) {
          // try to assign the new BN to the generator
          try {
            const BayesNet< GUM_SCALAR >* bn = &(gen->getBayesNet());
            old_bns.insert(gen, bn);
            gen->setBayesNet(new_bn);
          } catch (...) {
            // if we could not assign the new BN to the generator, then
            // make all the generators that were successfully assigned this
            // BN revert to the old BN they had
            for (auto& generator: old_bns) {
              generator.first->setBayesNet(*(generator.second));
            }
            throw;
          }
        }
      }
    }


    /// returns the number of generators (alias for nbGenerators)
    template < template < typename > class ALLOC >
    INLINE std::size_t DBRowGeneratorSet< ALLOC >::size() const noexcept {
      return nb_generators__;
    }


    /// returns true if there are still rows that can be output by the RowFilter
    template < template < typename > class ALLOC >
    INLINE bool DBRowGeneratorSet< ALLOC >::hasRows() {
      return output_row__ != nullptr;
    }


    // try to produce a new row
    template < template < typename > class ALLOC >
    bool DBRowGeneratorSet< ALLOC >::produceNextRow__(
       const DBRow< DBTranslatedValue, ALLOC >* input_row, std::size_t i) {
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
      const DBRow< DBTranslatedValue, ALLOC >* row = input_row;
      while (i != nb_generators__) {
        auto generator = generators__[i];

        // if we did not pass any row yet to the ith generator, do it
        // else use method generate() to generate a new output row
        if (setInputRow_performed__[i] == 0) {
          // pass the current row
          const bool has_rows = generator->setInputRow(*row);

          // if the generator could not create output rows, try to backtrack
          if (!has_rows) {
            if (i > std::size_t(0)) {
              --i;
              continue;
            } else {
              // here we were unable to generate output rows
              output_row__ = nullptr;
              return false;
            }
          } else {
            // here, the generator is able to generate output rows
            // so, generate the first one
            row = &(generator->generate());
            setInputRow_performed__[i] = 1;

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
            setInputRow_performed__[i] = 0;
            if (i > std::size_t(0)) {
              --i;
              continue;
            } else {
              // here we were unable to generate output rows
              output_row__ = nullptr;
              return false;
            }
          }
        }
      }

      // here row contains a row generated on a leaf of the row generation tree
      // we should keep it when the user will ask for the next row to generate
      output_row__ = row;
      return true;
    }


    /// sets the input row from which the generator will create new rows
    template < template < typename > class ALLOC >
    INLINE bool DBRowGeneratorSet< ALLOC >::setInputRow(
       const DBRow< DBTranslatedValue, ALLOC >& input_row) {
      // reset all the generators: ask them to use method setInputDBRow
      if (hasRows())
        for (auto& performed: setInputRow_performed__)
          performed = 0;

      // now, parse the row generation tree
      return produceNextRow__(&input_row, std::size_t(0));
    }


    /// generate new rows from the input row
    template < template < typename > class ALLOC >
    INLINE const DBRow< DBTranslatedValue, ALLOC >&
                 DBRowGeneratorSet< ALLOC >::generate() {
      // get the row that we should return
      const auto row = output_row__;

      // we should ask the last generator to produce the next output row
      produceNextRow__(output_row__, nb_generators__ - 1);

      return *row;
    }


    /// resets the filter
    template < template < typename > class ALLOC >
    INLINE void DBRowGeneratorSet< ALLOC >::reset() {
      for (auto gen: generators__)
        gen->reset();
      for (auto& performed: setInputRow_performed__)
        performed = 0;
      output_row__ = nullptr;
    }


    /// sets the columns of interest: the output DBRow needs only
    /// contain values fot these columns
    template < template < typename > class ALLOC >
    INLINE void DBRowGeneratorSet< ALLOC >::setColumnsOfInterest(
       const std::vector< std::size_t, ALLOC< std::size_t > >& cols_of_interest) {
      // check that no output row generation is still active
      if (output_row__ != nullptr)
        GUM_ERROR(OperationNotAllowed,
                  "you cannot change the columns of interest while a "
                  "generation is still being processed");
      for (auto gen: generators__)
        gen->setColumnsOfInterest(cols_of_interest);
    }


    /// sets the columns of interest: the output DBRow needs only
    /// contain values fot these columns
    template < template < typename > class ALLOC >
    INLINE void DBRowGeneratorSet< ALLOC >::setColumnsOfInterest(
       std::vector< std::size_t, ALLOC< std::size_t > >&& cols_of_interest) {
      if (output_row__ != nullptr)
        GUM_ERROR(OperationNotAllowed,
                  "you cannot change the columns of interest while a "
                  "generation is still being processed");
      for (auto gen: generators__)
        gen->setColumnsOfInterest(cols_of_interest);
    }


    /// returns the current set of columns of interest
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::size_t, ALLOC< std::size_t > >&
                 DBRowGeneratorSet< ALLOC >::columnsOfInterest() const {
      if (nb_generators__ == std::size_t(0)) {
        GUM_ERROR(UndefinedElement,
                  "there are no generators yet in the generator set, so "
                  "there are no columns of interest");
      }
      return generators__[0]->columnsOfInterest();
    }


    /// returns the allocator used
    template < template < typename > class ALLOC >
    INLINE typename DBRowGeneratorSet< ALLOC >::allocator_type
       DBRowGeneratorSet< ALLOC >::getAllocator() const {
      return allocator_type(generators__.get_allocator());
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
