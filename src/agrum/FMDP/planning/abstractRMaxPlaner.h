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
 * @brief Headers of the RMax planer class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_ABSTRACT_RMAX_PLANER_H
#define GUM_ABSTRACT_RMAX_PLANER_H
// =========================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/planning/structuredPlaner.h>
// =========================================================================

namespace gum {  

  /**
   * @class AbstractRMaxPlaner AbstractRMaxPlaner.h <agrum/FMDP/planning/AbstractRMaxPlaner.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a RMax planning on given in parameter factored markov decision process
   *
   */
  template<typename GUM_SCALAR>
  class AbstractRMaxPlaner : public StructuredPlaner<GUM_SCALAR> {


      // ###################################################################
      /// @name
      // ###################################################################
      /// @{
    public :
        // ==========================================================================
        ///
        // ==========================================================================
        static AbstractRMaxPlaner<GUM_SCALAR>* ReducedAndOrderedInstance(GUM_SCALAR discountFactor = 0.9, GUM_SCALAR epsilon = 0.00001)
              { return new AbstractRMaxPlaner<GUM_SCALAR>( new MDDOperatorStrategy<GUM_SCALAR>(), discountFactor, epsilon);}

        // ==========================================================================
        ///
        // ==========================================================================
        static AbstractRMaxPlaner<GUM_SCALAR>* TreeInstance(GUM_SCALAR discountFactor = 0.9, GUM_SCALAR epsilon = 0.00001)
              { return new AbstractRMaxPlaner<GUM_SCALAR>( new TreeOperatorStrategy<GUM_SCALAR>(), discountFactor, epsilon);}

      /// @}

      // ###################################################################
      /// @name Constructor & destructor.
      // ###################################################################
      /// @{
  private:
        // ==========================================================================
        /// Default constructor
        // ==========================================================================
        AbstractRMaxPlaner( IOperatorStrategy<GUM_SCALAR>* opi, GUM_SCALAR discountFactor, GUM_SCALAR epsilon );

        // ==========================================================================
        /// Default destructor
        // ==========================================================================
  public:
        ~AbstractRMaxPlaner();

      /// @}



      // ###################################################################
      /// @name Planning Methods
      // ###################################################################
      /// @{

  public:

        // ==========================================================================
        /**
         * Initializes data structure needed for making the planning
         * @warning No calling this methods before starting the first makePlaninng
         * will surely and definitely result in a crash
         */
        // ==========================================================================
        virtual void initialize( FMDP<GUM_SCALAR>* fmdp );


        // ==========================================================================
        /**
         * Performs a value iteration
         *
         * @param nbStep : enables you to specify how many value iterations you wish to do.
         * makePlanning will then stop whether when optimal value function is reach or when nbStep have been performed
         */
        // ==========================================================================
        void makePlanning(Idx nbStep = 1000000);

      /// @}



      // ###################################################################
      /// @name Value Iteration Methods
      // ###################################################################
      /// @{

  protected:
        // ==========================================================================
        /// Performs a single step of value iteration
        // ==========================================================================
        virtual MultiDimFunctionGraph< GUM_SCALAR >* _valueIteration();

      /// @}



      // ###################################################################
      /// @name Optimal policy extraction methods
      // ###################################################################
      /// @{

  protected:
        // ==========================================================================
        /// Perform the required tasks to extract an optimal policy
        // ==========================================================================
        virtual void _evalPolicy ();

      /// @}

  protected:

    private :
        // ==========================================================================
        /// The threshold value
        /// Whenever | V^{n} - V^{n+1} | < threshold, we consider that V ~ V*
        // ==========================================================================
        GUM_SCALAR __threshold;

        bool __firstTime;
  };

} /* namespace gum */


#include <agrum/FMDP/planning/abstractRMaxPlaner.tcc>

#endif // GUM_ABSTRACT_RMAX_PLANER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

