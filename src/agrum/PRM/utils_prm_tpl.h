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


#include <agrum/PRM/utils_prm.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    Potential< GUM_SCALAR >*
       copyPotential(const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
                     const Potential< GUM_SCALAR >&                                       source) {
      const MultiDimImplementation< GUM_SCALAR >* impl = source.content();
      Potential< GUM_SCALAR >*                    p    = 0;

      try {
        if (dynamic_cast< const MultiDimReadOnly< GUM_SCALAR >* >(impl)) {
          if (dynamic_cast< const MultiDimNoisyORCompound< GUM_SCALAR >* >(impl)) {
            p = new Potential< GUM_SCALAR >(new MultiDimNoisyORCompound< GUM_SCALAR >(
               bij,
               static_cast< const MultiDimNoisyORCompound< GUM_SCALAR >& >(*impl)));
          } else if (dynamic_cast< const MultiDimNoisyORNet< GUM_SCALAR >* >(impl)) {
            p = new Potential< GUM_SCALAR >(new MultiDimNoisyORNet< GUM_SCALAR >(
               bij,
               static_cast< const MultiDimNoisyORNet< GUM_SCALAR >& >(*impl)));
          } else if (dynamic_cast< const aggregator::MultiDimAggregator< GUM_SCALAR >* >(impl)) {
            p = new Potential< GUM_SCALAR >(
               static_cast< MultiDimImplementation< GUM_SCALAR >* >(impl->newFactory()));

            for (auto var: impl->variablesSequence())
              p->add(*(bij.second(var)));
          } else if (dynamic_cast< const MultiDimBucket< GUM_SCALAR >* >(impl)) {
            // This is necessary just to prevent non initialized arrays
            const_cast< MultiDimBucket< GUM_SCALAR >* >(
               static_cast< const MultiDimBucket< GUM_SCALAR >* >(impl))
               ->compute();

            try {
              p = new Potential< GUM_SCALAR >(new MultiDimBijArray< GUM_SCALAR >(
                 bij,
                 static_cast< const MultiDimBucket< GUM_SCALAR >* >(impl)->bucket()));
            } catch (OperationNotAllowed const&) {
              // This is an empty bucket, it happens if all variables were
              // eliminated
              return new Potential< GUM_SCALAR >();
            }
          } else {
            GUM_ERROR(FatalError, "encountered an unexpected MultiDim implementation")
          }
        } else {
          if (dynamic_cast< const MultiDimArray< GUM_SCALAR >* >(impl)) {
            p = new Potential< GUM_SCALAR >(new MultiDimBijArray< GUM_SCALAR >(
               bij,
               static_cast< const MultiDimArray< GUM_SCALAR >& >(*impl)));
          } else if (dynamic_cast< const MultiDimBijArray< GUM_SCALAR >* >(impl)) {
            p = new Potential< GUM_SCALAR >(new MultiDimBijArray< GUM_SCALAR >(
               bij,
               static_cast< const MultiDimBijArray< GUM_SCALAR >& >(*impl)));
          } else if (dynamic_cast< const MultiDimSparse< GUM_SCALAR >* >(impl)) {
            GUM_ERROR(FatalError, "There is no MultiDimSparse in PRMs, normally...")
          } else {
            // Just need to make the copy using the bijection but we only use
            // multidim array
            GUM_ERROR(FatalError, "encountered an unexpected MultiDim implementation")
          }
        }

        return p;
      } catch (Exception const&) {
        if (p) delete p;

        throw;
      }
    }

    // the function used to combine two tables
    template < typename GUM_SCALAR >
    Potential< GUM_SCALAR > multPotential(const Potential< GUM_SCALAR >& t1,
                                          const Potential< GUM_SCALAR >& t2) {
      return t1 * t2;
    }

    template < typename GUM_SCALAR >
    void eliminateNode(const DiscreteVariable*          var,
                       Set< Potential< GUM_SCALAR >* >& pool,
                       Set< Potential< GUM_SCALAR >* >& trash) {
      Potential< GUM_SCALAR >* pot = nullptr;
      Potential< GUM_SCALAR >* tmp = nullptr;

      Set< const DiscreteVariable* > var_set;
      var_set.insert(var);
      Set< const Potential< GUM_SCALAR >* > pots;

      for (const auto p: pool)
        if (p->contains(*var)) pots.insert(p);

      if (pots.size() == 0) {
        return;
      } else if (pots.size() == 1) {
        tmp = const_cast< Potential< GUM_SCALAR >* >(*pots.begin());
        pot = new Potential< GUM_SCALAR >(tmp->margSumOut(var_set));
      } else {
        MultiDimCombinationDefault< Potential< GUM_SCALAR > > Comb(multPotential);
        tmp = Comb.execute(pots);
        pot = new Potential< GUM_SCALAR >(tmp->margSumOut(var_set));
        delete tmp;
      }

      for (const auto p: pots) {
        pool.erase(const_cast< Potential< GUM_SCALAR >* >(p));

        if (trash.exists(const_cast< Potential< GUM_SCALAR >* >(p))) {
          trash.erase(const_cast< Potential< GUM_SCALAR >* >(p));
          delete const_cast< Potential< GUM_SCALAR >* >(p);
        }
      }

      pool.insert(pot);
      trash.insert(pot);
    }

    template < typename GUM_SCALAR >
    void eliminateNodes(const std::vector< const DiscreteVariable* >& elim_order,
                        Set< Potential< GUM_SCALAR >* >&              pool,
                        Set< Potential< GUM_SCALAR >* >&              trash) {
      for (auto var: elim_order) {
        eliminateNode(var, pool, trash);
      }
    }

  } /* namespace prm */
}   // namespace gum
