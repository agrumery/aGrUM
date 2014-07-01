/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
 * @brief A pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and LocalSearchWithTabuList
 * 
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>
#include <agrum/learning/learningAlgoPack.h>


namespace gum {

  
  namespace learning {


    /// default constructor
    LearningAlgoPack::LearningAlgoPack () {
      GUM_CONSTRUCTOR ( LearningAlgoPack );
    }
      

    /// copy constructor
    LearningAlgoPack::LearningAlgoPack ( const LearningAlgoPack& from ) {
      GUM_CONS_CPY ( LearningAlgoPack );
    }
    

    /// move constructor
    LearningAlgoPack::LearningAlgoPack ( LearningAlgoPack&& from ) {
      GUM_CONS_MOV ( LearningAlgoPack );
    }

    
    /// destructor
    LearningAlgoPack::~LearningAlgoPack () {
      GUM_DESTRUCTOR ( LearningAlgoPack );
    }


    /// copy operator
    LearningAlgoPack&
    LearningAlgoPack::operator= ( const LearningAlgoPack& from ) {
      return *this;
    }


    /// move operator
    LearningAlgoPack&
    LearningAlgoPack::operator= ( LearningAlgoPack&& from ) {
      return *this;
    }


    /// reads a file and returns a databaseVectInRam
    DatabaseVectInRAM
    LearningAlgoPack::__readFile ( const std::string& filename ) {
      DatabaseFromCSV database ( filename );
      return database;
    }
    

    /// learn a structure from a file
    DAG LearningAlgoPack::learnDAG ( std::string filename ) {
      // read the database
      DatabaseVectInRAM database = __readFile ( filename );

      // create the RowFilter used for learning: we first generate a universal
      // filter that can parse any database. Then, we parse once the DB to
      // convert it into a compact int (an interval 0..N-1) so that we can
      // parse it very quickly
      DBRowTranslatorSetDynamic<CellTranslatorUniversal> raw_translators;
      raw_translators.insertTranslator ( Col<0> (), database.nbVariables () );

      auto generators = make_generators ( RowGeneratorIdentity () );
      
      auto raw_filter = make_DB_row_filter ( database, raw_translators,
                                             generators );
      
      DBTransformCompactInt transfo;
      transfo.transform ( raw_filter );

      DBRowTranslatorSetDynamic<CellTranslatorCompactIntId> fast_translators;
      fast_translators.insertTranslator ( Col<0> (), database.nbVariables () );
     
      auto fast_filter = make_DB_row_filter ( database, fast_translators,
                                              generators );

      // get the modalities and create the score
      std::vector<unsigned int> modalities = raw_filter.modalities ();
      __createScore ( fast_filter, modalities );

      return __learnDAG ( fast_filter, modalities );
    }

        
              


    /// sets an initial DAG structure
    void LearningAlgoPack::setInitialDAG ( const DAG& ) {
    }

  
    /// set the max number of changes decreasing the score that we allow to apply
    void LearningAlgoPack::setMaxNbDecreasingChanges ( unsigned int nb ) {
    }
    
  
  } /* namespace learning */
  
  
} /* namespace gum */

