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
 * @brief Abstract base class for computing triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <math.h>

#include <agrum/config.h>
#include <agrum/graphs/triangulations/triangulation.h>

namespace gum {

  /// constructor

  Triangulation::Triangulation() {
    // for debugging purposes
    GUM_CONSTRUCTOR(Triangulation);
  }

  /// destructor

  Triangulation::~Triangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(Triangulation);
  }

  double Triangulation::maxLog10CliqueDomainSize() {
    double res = 0.0;
    double dSize;
    const JunctionTree &jt = junctionTree();

    for (const auto cl : jt.nodes()) {
      dSize = 0.0;

      for (const auto nod : jt.clique(cl))
        dSize += std::log10(_modalities[nod]);

      if (res < dSize)
        res = dSize;
    }

    return res;
  }

  /// returns the modalities of the variables of the graph to be triangulated

  const NodeProperty<Size> &Triangulation::modalities() const { return _modalities; }

} /* namespace gum */
