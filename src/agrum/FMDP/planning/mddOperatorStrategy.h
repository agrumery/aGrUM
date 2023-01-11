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


/**
 * @file
 * @brief Headers of the MDDOperatorStrategy planer class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#ifndef GUM_MDD_OPERATOR_STRATEGY_H
#define GUM_MDD_OPERATOR_STRATEGY_H
// =========================================================================
#include <agrum/FMDP/planning/IOperatorStrategy.h>
// =========================================================================

namespace gum {

  /**
   * @class MDDOperatorStrategy mddOperatorStrategy.h
   * <agrum/FMDP/planning/mddOperatorStrategy.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a SPUDD planning on given in parameter factored markov decision
   * process
   *
   */
  template < typename GUM_SCALAR >
  class MDDOperatorStrategy: public IOperatorStrategy< GUM_SCALAR > {
    public:
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    /**
     * Default constructor
     */
    MDDOperatorStrategy();

    /**
     * Default destructor
     */
    ~MDDOperatorStrategy();

    /// @}


    // ###################################################################
    /// @name
    // ###################################################################
    /// @{

    MultiDimFunctionGraph< GUM_SCALAR, ExactTerminalNodePolicy >* getFunctionInstance() {
      return MultiDimFunctionGraph< GUM_SCALAR >::getReducedAndOrderedInstance();
    }
    MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
       getArgMaxFunctionInstance() {
      return MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >,
                                    SetTerminalNodePolicy >::getReducedAndOrderedInstance();
    }
    MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >* getAggregatorInstance() {
      return MultiDimFunctionGraph< ActionSet,
                                    SetTerminalNodePolicy >::getReducedAndOrderedInstance();
    }

    /// @}

    // ###################################################################
    /// @name Graph Function Operations Methods
    // ###################################################################
    /// @{

    protected:
    // ==========================================================================
    /**
     *  Computes Qaction for given actionid
     */
    // ==========================================================================
    MultiDimFunctionGraph< GUM_SCALAR >* regress(const MultiDimFunctionGraph< GUM_SCALAR >* Vold,
                                                 Idx                                   actionId,
                                                 const FMDP< GUM_SCALAR >*             fmdp,
                                                 const Set< const DiscreteVariable* >& elVarSeq);

    // ==========================================================================
    /// @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >*
       maximize(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                Idx                                        del = 3);

    // ==========================================================================
    /// @warning Minimze the two given functions
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >*
       minimize(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                Idx                                        del = 3);

    // ==========================================================================
    /// @warning given f1 and f2 are deleted, returns the new one
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >*
       multiply(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                Idx                                        del = 3);

    // ==========================================================================
    /// @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    virtual MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
       argmaximize(
          const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f1,
          const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f2,
          Idx del = 3);

    // ==========================================================================
    /// @warning given function is deleted, returns the new one
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >* add(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                     const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                     Idx del = 1);

    // ==========================================================================
    /// @warning this time, nothing is deleted
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >*
       subtract(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                Idx                                        del = 0);


    /// @}

    protected:
    // ==========================================================================
    /// Indicates if whether or not given var is to be eliminated.
    /// Called by the evalQaction.
    // ==========================================================================
    INLINE bool shouldEleminateVar_(const DiscreteVariable* v, const FMDP< GUM_SCALAR >* fmdp) {
      return v == nullptr ? false : fmdp->mapMainPrime().existsSecond(v);
    }

    // ==========================================================================
    /// Returns the last var in the var order for given graph function
    /// Called by the evalQaction.
    // ==========================================================================
    INLINE const DiscreteVariable* lastVar_(const MultiDimFunctionGraph< GUM_SCALAR >* function) {
      return function->variablesSequence().size() == 0
              ? nullptr
              : function->variablesSequence().atPos(function->variablesSequence().size() - 1);
    }
  };
} /* namespace gum */


#include <agrum/FMDP/planning/mddOperatorStrategy_tpl.h>

#endif   // GUM_MDD_OPERATOR_STRATEGY_H
