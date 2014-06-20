/***************************************************************************
 *  Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN   *
 *  {prenom.nom}_at_lip6.fr                                                *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.               *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of the ModelLearner class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */


// =========================================================================
#include <agrum/FMDP/learning/spimddi.h>
// =========================================================================

namespace gum {

    // ==========================================================================
    // Constructor & destructor.
    // ==========================================================================

    // ###################################################################
    /// Default constructor
    // ###################################################################
    template<typename GUM_SCALAR>
    SPIMDDI<GUM_SCALAR>::SPIMDDI (GUM_SCALAR discountFactor, double epsilon, double learningThreshold, Idx nbStep){

        __fmdp = new FactoredMarkovDecisionProcess<GUM_SCALAR>();
        __fmdp->setDiscount(discountFactor);

        __learner = new FMDPLearner<GUM_SCALAR>(__fmdp, learningThreshold);

    }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    template<typename GUM_SCALAR>
    SPIMDDI<GUM_SCALAR>::~SPIMDDI (){

    }

    /// @}

    // ==========================================================================
    /// @name
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    template<typename GUM_SCALAR>
    void SPIMDDI<GUM_SCALAR>::addAction( Idx actionId, std::string actionName ){

    }

    template<typename GUM_SCALAR>
    void SPIMDDI<GUM_SCALAR>::addVariable( const DiscreteVariable* var ){

    }

    /// @}

    // ==========================================================================
    /// @name
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    template<typename GUM_SCALAR>
    void SPIMDDI<GUM_SCALAR>::feedback( const Instantiation* newState, Idx reward){

    }

    // ###################################################################
    ///
    // ###################################################################
    template<typename GUM_SCALAR>
    Idx SPIMDDI<GUM_SCALAR>::askForAction( ){
        return 0;
    }


} // End of namespace gum
