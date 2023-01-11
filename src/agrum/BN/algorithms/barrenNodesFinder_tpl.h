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


namespace gum {

  /// returns the set of barren potentials in messages sent in a junction tree
  template < typename GUM_SCALAR >
  ArcProperty< Set< const Potential< GUM_SCALAR >* > >
     BarrenNodesFinder::barrenPotentials(const CliqueGraph&             junction_tree,
                                         const IBayesNet< GUM_SCALAR >& bn) {
    // get the barren nodes
    ArcProperty< NodeSet > barren_nodes = this->barrenNodes(junction_tree);

    // transform the node sets into sets of potentials
    ArcProperty< Set< const Potential< GUM_SCALAR >* > > result;
    for (const auto& barren: barren_nodes) {
      Set< const Potential< GUM_SCALAR >* > potentials;
      for (const auto node: barren.second) {
        potentials.insert(&(bn.cpt(node)));
      }
      result.insert(Arc(barren.first), std::move(potentials));
    }

    return result;
  }


} /* namespace gum */
