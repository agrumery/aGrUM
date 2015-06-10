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
 * @brief Source implementation of Base classes for oriented graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/graphs/diGraph.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/diGraph.inl>
#endif // GUM_NOINLINE

namespace gum {

  DiGraph::DiGraph(Size nodes_size, bool nodes_resize_policy, Size arcs_size,
                   bool arcs_resize_policy)
      : NodeGraphPart(nodes_size, nodes_resize_policy),
        ArcGraphPart(arcs_size, arcs_resize_policy) {
    GUM_CONSTRUCTOR(DiGraph);
  }

  DiGraph::DiGraph(const DiGraph &g) : NodeGraphPart(g), ArcGraphPart(g) {
    GUM_CONS_CPY(DiGraph);
  }

  DiGraph::~DiGraph() { GUM_DESTRUCTOR(DiGraph); }

  const std::string DiGraph::toString() const {
    std::string s = NodeGraphPart::toString();
    s += " , ";
    s += ArcGraphPart::toString();
    return s;
  }

  const std::string DiGraph::toDot(const std::string &name) const {
    std::stringstream strBuff;
    std::string tab = "     ";
    strBuff << "digraph " << name << " {" << std::endl;

    for (const auto node : nodes())
      strBuff << tab << node << ";" << std::endl;

    strBuff << std::endl;

    for (const auto &arc : arcs())
      strBuff << tab << arc.tail() << " -> " << arc.head() << ";" << std::endl;

    strBuff << "}" << std::endl << std::endl;
    return strBuff.str();
  }

  /// for friendly displaying the content of directed graphs
  std::ostream &operator<<(std::ostream &stream, const DiGraph &g) {
    stream << g.toString();
    return stream;
  }

} /* namespace gum */
