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

#include <algorithm>

#include <agrum/config.h>
#include <agrum/learning/BNLearner.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/BNLearner.inl>
#endif /* GUM_NO_INLINE */


namespace gum {

  
  namespace learning {


    /// default constructor
    BNLearner::BNLearner () {
      GUM_CONSTRUCTOR ( BNLearner );
    }
      

    /// copy constructor
    BNLearner::BNLearner ( const BNLearner& from ) {
      GUM_CONS_CPY ( BNLearner );
    }
    

    /// move constructor
    BNLearner::BNLearner ( BNLearner&& from ) {
      GUM_CONS_MOV ( BNLearner );
    }

    
    /// destructor
    BNLearner::~BNLearner () {
      GUM_DESTRUCTOR ( BNLearner );
    }


    /// copy operator
    BNLearner&
    BNLearner::operator= ( const BNLearner& from ) {
      return *this;
    }


    /// move operator
    BNLearner&
    BNLearner::operator= ( BNLearner&& from ) {
      return *this;
    }



    


    /// reads a file and returns a databaseVectInRam
    DatabaseVectInRAM
    BNLearner::__readFile ( const std::string& filename ) {
      // get the extension of the file
      int filename_size = filename.size ();
      if ( filename_size < 4 ) {
        GUM_ERROR ( FormatNotFound, "BNLearner could not determine the "
                    "file type of the database" );
      }
      std::string extension = filename.substr ( filename.size () - 4 );
      std::transform( extension.begin (), extension.end (),
                      extension.begin (), ::tolower );
      
      if ( extension == ".csv" ) {
        return DatabaseFromCSV ( filename );
      }

      GUM_ERROR ( OperationNotAllowed,
                  "BNLearner does not support yet this type of database file" );
    }
    

    /// learn a structure from a file
    DAG BNLearner::learnDAG ( std::string filename ) {
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
      
      DBTransformCompactInt raw2fast_transfo;
      raw2fast_transfo.transform ( raw_filter );

      DBRowTranslatorSetDynamic<CellTranslatorCompactIntId> fast_translators;
      fast_translators.insertTranslator ( Col<0> (), database.nbVariables () );
     
      auto fast_filter = make_DB_row_filter ( database, fast_translators,
                                              generators );

      // get the modalities and create the score
      std::vector<unsigned int> modalities = raw_filter.modalities ();
      __createScore ( fast_filter, modalities );

      return __learnDAG ( fast_filter, modalities );
    }
    
  
  } /* namespace learning */
  
  
} /* namespace gum */

