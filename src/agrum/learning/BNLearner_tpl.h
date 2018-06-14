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
 * The pack currently contains K2, GreedyHillClimbing and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <fstream>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to help IDE parser
#  include <agrum/learning/BNLearner.h>

#  include <agrum/learning/BNLearnUtils/BNLearnerListener.h>

namespace gum {

  namespace learning {
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(
      const std::string&                filename,
      const std::vector< std::string >& missing_symbols) :
        genericBNLearner(filename, missing_symbols) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(const DatabaseTable<>& db) :
        genericBNLearner(db) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(
      const std::string&                 filename,
      const gum::BayesNet< GUM_SCALAR >& bn,
      const std::vector< std::string >&  missing_symbols) :
        genericBNLearner(filename, bn, missing_symbols)
        , __dag4learnParams (bn.dag() ) {
      GUM_CONSTRUCTOR(BNLearner)
    }

    /// copy constructor
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(const BNLearner< GUM_SCALAR >& src) :
      genericBNLearner(src),
      __dag4learnParams ( src.__dag4learnParams ) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    /// move constructor
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::BNLearner(BNLearner< GUM_SCALAR >&& src) :
      genericBNLearner(src),
      __dag4learnParams ( src.__dag4learnParams ) {
      GUM_CONSTRUCTOR(BNLearner);
    }

    /// destructor
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >::~BNLearner() {
      GUM_DESTRUCTOR(BNLearner);
    }

    /// @}

    // ##########################################################################
    /// @name Operators
    // ##########################################################################
    /// @{

    /// copy operator
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >& BNLearner< GUM_SCALAR >::
                             operator=(const BNLearner< GUM_SCALAR >& src) {
      genericBNLearner::operator=(src);
      __dag4learnParams = src.__dag4learnParams;
      return *this;
    }

    /// move operator
    template < typename GUM_SCALAR >
    BNLearner< GUM_SCALAR >& BNLearner< GUM_SCALAR >::
                             operator=(BNLearner< GUM_SCALAR >&& src) {
      genericBNLearner::operator=(std::move(src));
      __dag4learnParams = src.__dag4learnParams;
      return *this;
    }

    /// learn a Bayes Net from a file
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR > BNLearner< GUM_SCALAR >::learnBN() {
      // create the score, the apriori and the estimator
      __createApriori();
      __createScore();
      __createParamEstimator();

      return DAG2BNLearner::
        createBN< GUM_SCALAR, ParamEstimator<>, DBTranslatorSet<> >(
          *__param_estimator,
          __learnDAG(),
          __score_database.names(),
          __score_database.modalities(),
          __score_database.databaseTable().translatorSet());
    }

    /// learns a BN (its parameters) when its structure is known
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR >
      BNLearner< GUM_SCALAR >::learnParameters(const DAG& dag,
                                               bool take_into_account_score) {
      // if the dag contains no node, return an empty BN
      if ( ! dag.size() )
        return BayesNet< GUM_SCALAR > ();
        
      // check that the dag corresponds to the database
      std::vector<NodeId> ids;
      ids.reserve ( dag.sizeNodes() );
      for ( const auto node : dag ) ids.push_back ( node );
      std::sort ( ids.begin(), ids.end() );

      if ( ids.back() >= __score_database.names().size () ) {
        std::stringstream str;
        str << "Learning parameters corresponding to the dag is impossible "
            << "because the database does not contain the following nodeID";
        std::vector<NodeId> bad_ids;
        for ( const auto node : ids ) {
          if ( node >= __score_database.names().size () )
            bad_ids.push_back ( node );
        }
        if ( bad_ids.size() > 1 ) str << 's';
        str << ": ";
        bool deja = false;
        for ( const auto node : bad_ids ) {
          if ( deja ) str << ", ";
          else deja = true;
          str << node;
        }
        GUM_ERROR ( MissingVariableInDatabase, str.str() );
      }

      // create the apriori and the estimator
      __createApriori();
      __createParamEstimator(take_into_account_score);

      return DAG2BNLearner::
        createBN< GUM_SCALAR, ParamEstimator<>, DBTranslatorSet<> >(
          *__param_estimator,
          dag,
          __score_database.names(),
          __score_database.modalities(),
          __score_database.databaseTable().translatorSet());
    }

    
    /// learns a BN (its parameters) when its structure is known
    template < typename GUM_SCALAR >
    BayesNet< GUM_SCALAR >
      BNLearner< GUM_SCALAR >::learnParameters(bool take_into_account_score) {
      return learnParameters( __dag4learnParams, take_into_account_score );
    }
      

    template < typename GUM_SCALAR >
    NodeProperty< Sequence< std::string > >
      BNLearner< GUM_SCALAR >::__labelsFromBN(const std::string& filename,
                                              const BayesNet< GUM_SCALAR >& src) {
      std::ifstream in(filename, std::ifstream::in);

      if ((in.rdstate() & std::ifstream::failbit) != 0) {
        GUM_ERROR(gum::IOError, "File " << filename << " not found");
      }

      CSVParser<> parser(in);
      parser.next();
      auto names = parser.current();

      NodeProperty< Sequence< std::string > > modals;

      for (gum::Idx col = 0; col < names.size(); col++) {
        try {
          gum::NodeId graphId = src.idFromName(names[col]);
          modals.insert(col, gum::Sequence< std::string >());

          for (gum::Size i = 0; i < src.variable(graphId).domainSize(); ++i)
            modals[col].insert(src.variable(graphId).label(i));
        } catch (const gum::NotFound&) {
          // no problem : a column which is not in the BN...
        }
      }

      return modals;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
