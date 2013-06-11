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
 * @brief the class for all the scores that are like BIC, etc, whose
 * formula is asymmetric w.r.t. the nodes contained in the formula.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_ASYMMETRIC_SCORE_H
#define GUM_LEARNING_ASYMMETRIC_SCORE_H


#include <vector>
#include <agrum/config.h>
#include <agrum/learning/score.h>


namespace gum {

  
  namespace learning {

    
    class AsymmetricScore : public Score {
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      AsymmetricScore ( const Database& database );

      /// destructor
      ~AsymmetricScore ();

      /// @}

      
    protected:
      /// the single ids deduced from pairs of ids, that are needed for the score  
      std::vector<unsigned int> _db_induced_ids;
      
      /// a function that determines the db single ids needed for the score
      void _computeInducedSingleIds
      ( const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids );

      /// computes the "unconditional" counting tree for a set of single targets
      /** The nodes are identified by their id in the database.
       * @warning the method uses a reference to db_single_ids and does
       * not copy it in order to speed-up computations. Be careful not to
       * delete this vector from memory before calling the score method. */
      void _createTree ( const std::vector<unsigned int>& db_single_ids );

      /// compute the counting tree of the set of targets conditioned on some nodes
      /** The nodes are identified by their id in the database.
       * @warning the method uses a reference to db_single_ids and
       * db_conditioning_ids, and it does not copy these vectors in order to
       * speed-up computations. So, be careful not to delete these vectors from
       * memory before calling the score method.*/
      void _createTree ( const std::vector<unsigned int>& db_conditioning_ids,
                         const std::vector<unsigned int>& db_single_ids );

      /// computes the "unconditional" counting tree of a set of pairs of targets
      /** The nodes are identified by their id in the database.
       * @warning the method uses a reference to db_pair_ids and does
       * not copy it in order to speed-up computations. Be careful not to
       * delete this vector from memory before calling the score method. */
      void _createTree
      ( const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids );
      
      /// compute the counting tree of the set of targets conditioned on some nodes
      /** The nodes are identified by their id in the database.
       * @warning the method uses a reference to db_pair_ids and
       * db_conditioning_ids, and it does not copy these vectors in order to
       * speed-up computations. So, be careful not to delete these vectors from
       * memory before calling the score method.*/
      void _createTree
      ( const std::vector<unsigned int>& db_conditioning_ids,
        const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids );


    private:
      /// an empty set of conditioning nodes
      std::vector<unsigned int> __empty_cond_set;

      /// an empty set of target pairs
      std::vector< std::pair<unsigned int, unsigned int> > __empty_pair_set;
      
    };


  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/asymmetricScore.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_ASYMMETRIC_SCORE_H */
