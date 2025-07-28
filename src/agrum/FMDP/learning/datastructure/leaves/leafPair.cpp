/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Sources for concrete leaf class
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */
// =======================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafPair.h>

#include <agrum/base/core/math/math_utils.h>

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
