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


/**
 * @file
 * @brief Sources for concrete leaf class
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */
// =======================================================
#include <agrum/tools/core/math/math.h>
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafPair.h>
// =======================================================


namespace gum {

  // ############################################################################
  // Miscelleanous Methods
  // ############################################################################

  void LeafPair::updateLikelyhood() {
    likelyhood1__ = 0.0;
    likelyhood2__ = 0.0;

    if (!l1__->total() || !l2__->total()) {
      likelyhood1__ = std::numeric_limits< double >::max();
      likelyhood2__ = std::numeric_limits< double >::max();
      return;
    }

    double scaleFactor1 = l1__->total() / (l1__->total() + l2__->total());
    double scaleFactor2 = l2__->total() / (l1__->total() + l2__->total());
    for (Idx moda = 0; moda < l1__->nbModa(); ++moda) {
      if (l1__->effectif(moda)) {
        double add =
           l1__->effectif(moda)
           * std::log(
              l1__->effectif(moda)
              / (scaleFactor1 * (l1__->effectif(moda) + l2__->effectif(moda))));
        likelyhood1__ += add;
      }
      if (l2__->effectif(moda)) {
        double add =
           l2__->effectif(moda)
           * std::log(
              l2__->effectif(moda)
              / (scaleFactor2 * (l1__->effectif(moda) + l2__->effectif(moda))));
        likelyhood2__ += add;
      }
    }

    likelyhood1__ *= 2;
    likelyhood2__ *= 2;
  }

  double LeafPair::likelyhood() {
    //      updateLikelyhood();
    return 1
           - ChiSquare::probaChi2(likelyhood1__ > likelyhood2__ ? likelyhood1__
                                                                : likelyhood2__,
                                  (l1__->nbModa() - 1));
  }

  std::string LeafPair::toString() {
    std::stringstream ss;
    ss << "\t[  Leaf1 : " << l1__->toString() << " - Leaf2 : " << l2__->toString();
    //      ss << " - L1 Total : " << l1__->total() << " - L2 Total : " <<
    //      l2__->total();
    //      for( Idx moda = 0; moda < l1__->nbModa(); ++moda )
    //        ss << "~ M=" << moda << ".L1=" << l1__->effectif(moda) << ".L2="
    //        << l2__->effectif(moda) << " ~";
    ss << " - GStat : " << this->likelyhood() << " ]";
    return ss.str();
  }

}   // namespace gum
