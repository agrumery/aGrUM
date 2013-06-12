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
 * @brief the class for all the independence tests with symmetric scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_SYMMETRIC_INDEPENDENCE_TEST_H
#define GUM_LEARNING_SYMMETRIC_INDEPENDENCE_TEST_H


namespace gum {

  
  namespace learning {

    
    template<class SCORE> class SymmetricIndependenceTest :
    public IndependenceTest {
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      SymmetricIndependenceTest ( const Database& database );

      /// destructor
      ~SymmetricIndependenceTest ();

      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// set a new set of conditioning and target pairs to test
      void setNodes
      ( const std::vector<unsigned int>& db_conditioning_ids,
        const std::vector< std::pair<unsigned int,unsigned int> >& db_pair_ids );

      /// set a new set of target pairs to test
      void setTargetNodes
      ( const std::vector< std::pair<unsigned int,unsigned int> >& db_pair_ids );

      /// set a new set of conditioning and target single nodes to test
      void setNodes
      ( const std::vector<unsigned int>& db_conditioning_ids,
        const std::vector<unsigned int>& db_single_ids );

      /// set a new set of target pairs to test
      void setTargetNodes
      ( const std::vector< std::pair<unsigned int,unsigned int> >& db_pair_ids );

      
      /// returns the score of a given pair X,Y given the conditioning nodes
      float score ( const std::pair<unsigned int,unsigned int>& XY_pair ) const;

      /// returns the score of a given pair X,Y given the conditioning nodes
      float score ( unsigned int X, unsigned int Y ) const;

      /// @}


    private:
      /// a pointer to the database
      Database* __database;
      
      /// the counting tree used for the computations
      CountingTree __tree;
    };


  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* GUM_LEARNING_SYMMETRIC_INDEPENDENCE_TEST_H */
