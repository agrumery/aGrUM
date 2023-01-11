/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// inserts a new generator at the end of the set
    template < class Generator >
    void DBRowGeneratorSet::insertGenerator(const Generator& generator) {
      // check that no output row generation is still active
      if (_output_row_ != nullptr)
        GUM_ERROR(OperationNotAllowed,
                  "you cannot insert a new generator while a generation is "
                  "still being processed");

      _generators_.push_back(generator.clone());

      try {
        _setInputRow_performed_.push_back(0);
      } catch (...) {
        delete _generators_.back();
        _generators_.pop_back();
        throw;
      }

      ++_nb_generators_;
      _output_row_ = nullptr;
    }


    /// inserts a new generator at the end of the set
    template < class Generator >
    void DBRowGeneratorSet::insertGenerator(const Generator& generator, const std::size_t i) {
      // check that no output row generation is still active
      if (_output_row_ != nullptr)
        GUM_ERROR(OperationNotAllowed,
                  "you cannot insert a new generator while a generation is "
                  "still being processed");

      _generators_.insert(_generators_.begin() + i, generator.clone());

      try {
        _setInputRow_performed_.push_back(0);
      } catch (...) {
        delete *(_generators_.begin() + i);
        _generators_.erase(_generators_.begin() + i);
        throw;
      }

      ++_nb_generators_;
      _output_row_ = nullptr;
    }


    /// assign a new Bayes net to all the generators that depend on a BN
    template < typename GUM_SCALAR >
    void DBRowGeneratorSet::setBayesNet(const BayesNet< GUM_SCALAR >& new_bn) {
      HashTable< DBRowGeneratorWithBN< GUM_SCALAR >*, const BayesNet< GUM_SCALAR >* > old_bns;

      for (auto xgen: _generators_) {
        // check if the generator relies on a Bayes net
        DBRowGeneratorWithBN< GUM_SCALAR >* gen = nullptr;
        try {
          gen = dynamic_cast< DBRowGeneratorWithBN< GUM_SCALAR >* >(xgen);
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

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
