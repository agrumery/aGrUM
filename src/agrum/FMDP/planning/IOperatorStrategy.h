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
 * @brief Headers of the Operator Strategy interface.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


// =========================================================================
#ifndef GUM_SDYNA_OPERATOR_STRATEGY_H
#define GUM_SDYNA_OPERATOR_STRATEGY_H
// =========================================================================
// =========================================================================
#include <agrum/FMDP/planning/actionSet.h>
// =========================================================================
// =========================================================================

namespace gum {

  /**
   * @class IOperatorStrategy IOperatorStrategy.h
   * <agrum/FMDP/SDyna/IOperatorStrategy.h>
   * @brief Interface for manipulating Operator center
   * @ingroup fmdp_group
   *
   */
  template < typename GUM_SCALAR >
  class IOperatorStrategy {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Destructor (virtual and empty since it's an interface)
    // ==========================================================================
    virtual ~IOperatorStrategy() {}

    /// @}


    // ###################################################################
    /// @name
    // ###################################################################
    /// @{

    virtual MultiDimFunctionGraph< GUM_SCALAR, ExactTerminalNodePolicy >* getFunctionInstance() = 0;
    virtual MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
       getArgMaxFunctionInstance()
       = 0;
    virtual MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >* getAggregatorInstance() = 0;

    /// @}


    // ###################################################################
    /// @name Graph Function Operations Methods
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    /// Performs a multiplication/projection on given qAction
    /// @return qAction : the computed Q(s,a)
    /// @warning given qAction is deleted, return the new one
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >*
       regress(const MultiDimFunctionGraph< GUM_SCALAR >* Vold,
               Idx                                        actionId,
               const FMDP< GUM_SCALAR >*                  fmdp,
               const Set< const DiscreteVariable* >&      elVarSeq)
       = 0;

    // ==========================================================================
    /// @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >*
       maximize(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                Idx                                        del = 3)
       = 0;

    // ==========================================================================
    /// @warning Minimze the two given functions
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >*
       minimize(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                Idx                                        del = 3)
       = 0;

    // ==========================================================================
    /// @warning given f1 and f2 are deleted, returns the new one
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >*
       multiply(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                Idx                                        del = 3)
       = 0;

    // ==========================================================================
    /// @warning given vFunction and qAction are deleted, returns the new one
    // ==========================================================================
    virtual MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
       argmaximize(
          const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f1,
          const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f2,
          Idx del = 3)
       = 0;

    // ==========================================================================
    /// @warning given function is deleted, returns the new one
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >* add(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                                     const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                                     Idx del = 1)
       = 0;

    // ==========================================================================
    /// @warning this time, nothing is deleted
    // ==========================================================================
    virtual MultiDimFunctionGraph< GUM_SCALAR >*
       subtract(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                Idx                                        del = 0)
       = 0;


    /// @}

    protected:
    INLINE void deleteFunctionGraph_(const MultiDimFunctionGraph< GUM_SCALAR >* f1,
                                     const MultiDimFunctionGraph< GUM_SCALAR >* f2,
                                     Idx                                        del) {
      if (del == 1 || del == 3) delete f1;
      if (del >= 2) delete f2;
    }

    INLINE void deleteFunctionGraph_(
       const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f1,
       const MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >* f2,
       Idx                                                                                 del) {
      if (del == 1 || del == 3) delete f1;
      if (del >= 2) delete f2;
    }
  };
}   // namespace gum
#endif   // GUM_SDYNA_OPERATOR_STRATEGY_H
