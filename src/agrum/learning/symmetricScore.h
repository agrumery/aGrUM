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
 * @brief the class for all the scores whose formula is symmetric w.r.t. the
 * nodes it contains.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SYMMETRIC_SCORE_H
#define GUM_LEARNING_SYMMETRIC_SCORE_H


#include <vector>
#include <agrum/config.h>
#include <agrum/learning/score.h>


namespace gum {

  
  namespace learning {

    
    class SymmetricScore : public Score {
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      SymmetricScore ( const Database& database,
                       unsigned int max_tree_size = 0 );

      /// destructor
      ~SymmetricScore ();

      /// @}

      
    protected:
      /// a function that determines the db single ids needed for the score
      virtual void _computeInducedSingleIds
      ( const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids );
     
    };


  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/symmetricScore.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_SYMMETRIC_SCORE_H */
