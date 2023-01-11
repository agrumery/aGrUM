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
 * @brief Headers of the TreeOperatorStrategy planer class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_TreeOperatorStrategy_H
#define GUM_TreeOperatorStrategy_H
// =========================================================================
#include <agrum/FMDP/planning/IOperatorStrategy.h>
// =========================================================================

namespace gum {

  /**
   * @class TreeOperatorStrategy treeOperatorStrategy.h
   * <agrum/FMDP/planning/treeOperatorStrategy.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a TreeOperatorStrategy planning on given in parameter factored
   * markov decision process
   *
   */
  template < typename GUM_SCALAR >
  class TreeOperatorStrategy: public IOperatorStrategy< GUM_SCALAR > {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Default constructor
    // ==========================================================================
    TreeOperatorStrategy();

    // ==========================================================================
    /// Default destructor
    // ==========================================================================
    ~TreeOperatorStrategy();

    /// @}


    // ###################################################################
    /// @name
    // ###################################################################
    /// @{
    public:
    MultiDimFunctionGraph< GUM_SCALAR, ExactTerminalNodePolicy >* getFunctionInstance() {
      return MultiDimFunctionGraph< GUM_SCALAR >::getTreeInstance();
    }
    MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >, SetTerminalNodePolicy >*
       getArgMaxFunctionInstance() {
      return MultiDimFunctionGraph< ArgMaxSet< GUM_SCALAR, Idx >,
                                    SetTerminalNodePolicy >::getTreeInstance();
    }
    MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >* getAggregatorInstance() {
      return MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >::getTreeInstance();
    }

    /// @}

    // ###################################################################
    /// @name Graph Function Operations Methods
    // ###################################################################
    /// @{

    protected:
    // ==========================================================================
    /// Computes Qaction for given actionid
    /// @return qAction : the computed Q(s,a)
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
  };
} /* namespace gum */


#include <agrum/FMDP/planning/treeOperatorStrategy_tpl.h>

#endif   // GUM_TreeOperatorStrategy_H
