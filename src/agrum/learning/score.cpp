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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <cmath>
#include <agrum/learning/score.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/score.inl>
#endif /* GUM_NO_INLINE */


#define MAX_LOG2F_SIZE 100000


namespace gum {

  
  namespace learning {
    
    
    /// default constructor
    Score::Score ( const Database& database,
                   unsigned int max_tree_size ) :
      _database ( &database ),
      _tree ( database ),
      _db_conditioning_ids ( &__empty_cond_set ),
      _1log2 ( M_LOG2E ),
      __max_tree_size ( max_tree_size ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( Score );

      // resize the single scores so that it speeds-up computations
      _single_scores.resize ( 2 * _database->nbrNodes () );
      _pair_scores.resize ( 2 * _database->nbrNodes () );

      // fill the log2f
      unsigned int size = 1 + database.nbrLines ();
      if ( size > MAX_LOG2F_SIZE ) size = MAX_LOG2F_SIZE;
      __logf.resize ( size );
      __has_logf.resize ( size, false );

      // by default, log(0) = 0 because all scores compute things like xlog(x)
      __has_logf[0] = true;
      __logf[0] = 0.0f;
    }

    
    /// destructor
    Score::~Score () {
      // for debugging purposes
      GUM_DESTRUCTOR ( Score );
    }

    
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

