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
/**
 * @file
 * @brief Template implementations for the ContingencyTable class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/core/testPolicy/GTestPolicy.h>
// =========================================================================



namespace gum {


  // ##########################################################################
  //
  // ##########################################################################

    // ==========================================================================
    //
    // ==========================================================================
    template < typename GUM_SCALAR >
    void GTestPolicy<GUM_SCALAR>::addObservation( Idx iattr, GUM_SCALAR ivalue ) {
//      double fact = 0.0;

//      fact = (double)__conTab.attrBMarginal(ivalue) / (double)__nbObs;
//      for ( auto attrIter = __conTab.attrABeginSafe(); attrIter != __conTab.attrAEndSafe(); ++attrIter ){
//        double cell = (double)__conTab.joint(attrIter.key(), ivalue);
//        if(cell < 5)
//          continue;
//        double expected = (double)(attrIter.val()) * fact;
//        __GStat -= 2*cell*log(cell/expected);
//      }

//      fact = (double)__conTab.attrAMarginal(iattr) / (double)__nbObs;
//      for ( auto valIter = __conTab.attrBBeginSafe(); valIter != __conTab.attrBEndSafe(); ++valIter ) {
//        if(valIter.key() != ivalue){
//          double cell = (double)__conTab.joint(iattr, valIter.key());
//          if( cell < 5 )
//            continue;
//          double expected = fact * (double)(valIter.val());
//          __GStat -= 2*cell*log(cell/expected);
//        }
//      }

      __conTab.add( iattr, ivalue );
      __nbObs++;

//      fact = (double)__conTab.attrBMarginal(ivalue) / (double)__nbObs;
//      for ( auto attrIter = __conTab.attrABeginSafe(); attrIter != __conTab.attrAEndSafe(); ++attrIter ){
//        double cell = (double)__conTab.joint(attrIter.key(), ivalue);
//        if(cell < 5)
//          continue;
//        double expected = (double)(attrIter.val()) * fact;
//        __GStat += 2*cell*log(cell/expected);
//      }

//      fact = (double)__conTab.attrAMarginal(iattr) / (double)__nbObs;
//      for ( auto valIter = __conTab.attrBBeginSafe(); valIter != __conTab.attrBEndSafe(); ++valIter ) {
//        if(valIter.key() != ivalue){
//          double cell = (double)__conTab.joint(iattr, valIter.key());
//          if( cell < 5 )
//            continue;
//          double expected = fact * (double)(valIter.val());
//          __GStat += 2*cell*log(cell/expected);
//        }
//      }
    }


  // ############################################################################
  // @name Test result
  // ############################################################################

    // ============================================================================
    // Computes the GStat of current variable according to the test
    // ============================================================================
    template < typename GUM_SCALAR >
    void GTestPolicy<GUM_SCALAR>::computeState(){
      __GStat = 0;
//      std::cout << " - NB OBS : " << __nbObs << std::endl;
      for ( auto attrIter = __conTab.attrABeginSafe(); attrIter != __conTab.attrAEndSafe(); ++attrIter ){
//        std::cout << "\tattrKey : " << attrIter.key() << " - attrVal : " << attrIter.val();
        double semiExpected = (double)(attrIter.val())/(double)__nbObs;
//        std::cout << " => Semex : " << semiExpected << std::endl;
        for ( auto valIter = __conTab.attrBBeginSafe(); valIter != __conTab.attrBEndSafe(); ++valIter ) {
          double cell = (double)__conTab.joint(attrIter.key(),valIter.key());
          if( cell < 5 )
            continue;
//          std::cout << "\t\tvalKey : " << valIter.key() << " - valVal : " << valIter.val();
          double expected = semiExpected*(double)(valIter.val());
//          std::cout << " => Ex : " << expected << std::endl;
//          std::cout << "\t\tCell : " << cell << std::endl;

          __GStat += 2*cell*log(cell/expected);
//          std::cout << "\t\tNewGstat : " << __GStat << std::endl;
        }
      }
    }

} // End of namespace gum
