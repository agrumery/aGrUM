/*********************************************************************************
 * Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES *
 * {prenom.nom}_at_lip6.fr *
 *                                                                               *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or *
 * (at your option) any later version. *
 *                                                                               *
 * This program is distributed in the hope that it will be useful, *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
 * GNU General Public License for more details. *
 *                                                                               *
 * You should have received a copy of the GNU General Public License *
 * along with this program; if not, write to the *
 * Free Software Foundation, Inc., *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. *
 *********************************************************************************/
/**
* @file
* @brief Sources for concrete leaf class
*
* @author Jean-Christophe Magnan
*/
// =======================================================
#include <cmath>
// =======================================================
#include <agrum/FMDP/learning/core/chiSquare.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafPair.h>
// =======================================================


namespace gum {

  // ############################################################################
  // Miscelleanous Methods
  // ############################################################################

  void LeafPair::updateLikelyhood() {

    __likelyhood1 = 0.0;
    __likelyhood2 = 0.0;

    if ( !__l1->total() || !__l2->total() ) {
      __likelyhood1 = std::numeric_limits<double>::max();
      __likelyhood2 = std::numeric_limits<double>::max();
      return;
    }

    double scaleFactor1 = __l1->total() / ( __l1->total() + __l2->total() );
    double scaleFactor2 = __l2->total() / ( __l1->total() + __l2->total() );
    for ( Idx moda = 0; moda < __l1->nbModa(); ++moda ) {
      if ( __l1->effectif( moda ) ) {
        double add = __l1->effectif( moda ) *
                     std::log( __l1->effectif( moda ) /
                               ( scaleFactor1 * ( __l1->effectif( moda ) +
                                                  __l2->effectif( moda ) ) ) );
        __likelyhood1 += add;
      }
      if ( __l2->effectif( moda ) ) {
        double add = __l2->effectif( moda ) *
                     std::log( __l2->effectif( moda ) /
                               ( scaleFactor2 * ( __l1->effectif( moda ) +
                                                  __l2->effectif( moda ) ) ) );
        __likelyhood2 += add;
      }
    }

    __likelyhood1 *= 2;
    __likelyhood2 *= 2;
  }

  double LeafPair::likelyhood() {
    //      updateLikelyhood();
    return 1 - ChiSquare::probaChi2( __likelyhood1 > __likelyhood2 ? __likelyhood1
                                                                   : __likelyhood2,
                                     ( __l1->nbModa() - 1 ) );
  }

  std::string LeafPair::toString() {
    std::stringstream ss;
    ss << "\t[  Leaf1 : " << __l1->toString() << " - Leaf2 : " << __l2->toString();
    //      ss << " - L1 Total : " << __l1->total() << " - L2 Total : " <<
    //      __l2->total();
    //      for( Idx moda = 0; moda < __l1->nbModa(); ++moda )
    //        ss << "~ M=" << moda << ".L1=" << __l1->effectif(moda) << ".L2="
    //        << __l2->effectif(moda) << " ~";
    ss << " - GStat : " << this->likelyhood() << " ]";
    return ss.str();
  }

}  // end gum namespace
