/****************************************************************************
 *  Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES     *
 *  {prenom.nom}_at_lip6.fr                                                 *
 *                                                                          *
 *  This program is free software; you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation; either version 2 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program; if not, write to the                           *
 *  Free Software Foundation, Inc.,                                         *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                *
 ****************************************************************************/
/**
* @file
* @brief Class used to minimize simillary two function graphs
*
* @author Jean-Christophe Magnan
*/

// =======================================================
#ifndef GUM_GLOBAL_FUNCTION_GRAPH_MINIMIZER_H
#define GUM_GLOBAL_FUNCTION_GRAPH_MINIMIZER_H
// =======================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
// =======================================================
#include <agrum/graphs/diGraph.h>
// =======================================================

namespace gum {

  template <typename GUM_SCALAR,
            template <typename> class TerminalNodePolicy = ExactTerminalNodePolicy>
  class GlobalFunctionGraphMinimizer
  {
    public:
      GlobalFunctionGraphMinimizer( ) {
        GUM_CONSTRUCTOR(GlobalFunctionGraphMinimizer)
      }
      ~GlobalFunctionGraphMinimizer() {
        GUM_DESTRUCTOR(GlobalFunctionGraphMinimizer)
      }

      void loadFunctionGraphs(const MultiDimFunctionGraph<GUM_SCALAR,TerminalNodePolicy >* fg1, const MultiDimFunctionGraph<GUM_SCALAR,TerminalNodePolicy>* fg2);
      void minimize();

    private:

      void __extractVarList();
      void __orderSimillary();
      void __minimizeGlobally();

      MultiDimFunctionGraph<GUM_SCALAR,TerminalNodePolicy>* __fg1;
      MultiDimFunctionGraph<GUM_SCALAR,TerminalNodePolicy>* __fg2;

      Sequence<const DiscreteVariable*> __varList;
      Bijection<NodeId,const DiscreteVariable*> __dependancyMap;
      DiGraph __dependancyGraph;
    };
}

#include <agrum/multidim/FunctionGraphUtilities/globalFunctionGraphMinimizer.tcc>

#endif // GUM_GLOBAL_FUNCTION_GRAPH_MINIMIZER_H
