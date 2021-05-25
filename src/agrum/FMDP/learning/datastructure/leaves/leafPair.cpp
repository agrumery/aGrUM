/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafPair.h>
// =======================================================


namespace gum {

  // ############################################################################
  // Miscelleanous Methods
  // ############################################################################

  void LeafPair::updateLikelyhood() {
    _likelyhood1_ = 0.0;
    _likelyhood2_ = 0.0;

    if (!_l1_->total() || !_l2_->total()) {
      _likelyhood1_ = std::numeric_limits< double >::max();
      _likelyhood2_ = std::numeric_limits< double >::max();
      return;
    }

    double scaleFactor1 = _l1_->total() / (_l1_->total() + _l2_->total());
    double scaleFactor2 = _l2_->total() / (_l1_->total() + _l2_->total());
    for (Idx moda = 0; moda < _l1_->nbModa(); ++moda) {
      if (_l1_->effectif(moda)) {
        double add = _l1_->effectif(moda)
                   * std::log(_l1_->effectif(moda)
                              / (scaleFactor1 * (_l1_->effectif(moda) + _l2_->effectif(moda))));
        _likelyhood1_ += add;
      }
      if (_l2_->effectif(moda)) {
        double add = _l2_->effectif(moda)
                   * std::log(_l2_->effectif(moda)
                              / (scaleFactor2 * (_l1_->effectif(moda) + _l2_->effectif(moda))));
        _likelyhood2_ += add;
      }
    }

    _likelyhood1_ *= 2;
    _likelyhood2_ *= 2;
  }

  double LeafPair::likelyhood() {
    //      updateLikelyhood();
    return 1
         - ChiSquare::probaChi2(_likelyhood1_ > _likelyhood2_ ? _likelyhood1_ : _likelyhood2_,
                                (_l1_->nbModa() - 1));
  }

  std::string LeafPair::toString() {
    std::stringstream ss;
    ss << "\t[  Leaf1 : " << _l1_->toString() << " - Leaf2 : " << _l2_->toString();
    //      ss << " - L1 Total : " <<  _l1_->total() << " - L2 Total : " <<
    //       _l2_->total();
    //      for( Idx moda = 0; moda <  _l1_->nbModa(); ++moda )
    //        ss << "~ M=" << moda << ".L1=" <<  _l1_->effectif(moda) << ".L2="
    //        <<  _l2_->effectif(moda) << " ~";
    ss << " - GStat : " << this->likelyhood() << " ]";
    return ss.str();
  }

}   // namespace gum
