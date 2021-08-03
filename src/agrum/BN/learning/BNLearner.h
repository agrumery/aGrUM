/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief A basic pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and
 *LocalSearchWithTabuList
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_BN_LEARNER_H
#define GUM_LEARNING_BN_LEARNER_H

#include <sstream>
#include <algorithm>
#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/agrum.h>
#include <agrum/tools/core/bijection.h>
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/graphs/DAG.h>

#include <agrum/BN/learning/BNLearnUtils/genericBNLearner.h>

namespace gum {

  namespace learning {

    class BNLearnerListener;

    /** @class BNLearner
     * @brief A pack of learning algorithms that can easily be used
     *
     * The pack currently contains K2, GreedyHillClimbing and
     * LocalSearchWithTabuList
     * @ingroup learning_group
     */
    template < typename GUM_SCALAR >
    class BNLearner: public genericBNLearner {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /**
       * read the database file for the score / parameter estimation and var
       * names
       * @param filename the name of a CSV file containing the dataset
       * @param induceTypes By default, all the values in the dataset are
       * interpreted as "labels", i.e., as categorical values. But if some
       * columns of the dataset have only numerical values, it would certainly
       * be better totag them as corresponding to integer, range or continuous
       * variables. By setting induceTypes to true, this is precisely what the
       * BNLearner will do.
       * @param missingSymbols the set of symbols in the CSV that should
       * be interpreted as missing values
       */
      BNLearner(const std::string&                filename,
                const bool                        induceTypes    = true,
                const std::vector< std::string >& missingSymbols = {"?"});

      /// default constructor
      /**
       * ²read the database file for the score / parameter estimation and var
       * names
       * @param db an already initialized database table that is used to
       * fill the Database
       */
      BNLearner(const DatabaseTable<>& db);

      /**
       * Wrapper for BNLearner (filename,modalities,parse_database) using a bn
       * to find those modalities and nodeids.
       **/
      BNLearner(const std::string&                 filename,
                const gum::BayesNet< GUM_SCALAR >& src,
                const std::vector< std::string >&  missing_symbols = {"?"});

      /// copy constructor
      BNLearner(const BNLearner&);

      /// move constructor
      BNLearner(BNLearner&&);

      /// destructor
      virtual ~BNLearner();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      BNLearner& operator=(const BNLearner&);

      /// move operator
      BNLearner& operator=(BNLearner&&);

      /// @}

      /// learn a Bayes Net from a file (must have read the db before)
      BayesNet< GUM_SCALAR > learnBN();

      /// learns a BN (its parameters) when its structure is known
      /**
       * @param dag the structure of the Bayesian network
       * @param takeIntoAccountScore The dag passed in argument may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit apriori (like K2 which has a
       * 1-smoothing apriori), it is important to also take into account this
       * implicit apriori for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the apriori
       * specified by methods useAprioriXXX () + the implicit apriori of the
       * score, else we just take into account the apriori specified by
       * useAprioriXXX () */
      BayesNet< GUM_SCALAR > learnParameters(const DAG& dag, bool takeIntoAccountScore = true);

      // learns a BN (its parameters) when its structure is known
      /** @param take_into_account_score The dag of the BN which was passed in
       * argument to the BNLearner may have
       * been learnt from a structure learning. In this case, if the score used
       * to learn the structure has an implicit apriori (like K2 which has a
       * 1-smoothing apriori), it is important to also take into account this
       * implicit apriori for parameter learning. By default, if a score
       * exists, we will learn parameters by taking into account the apriori
       * specified by methods useAprioriXXX () + the implicit apriori of the
       * score, else we just take into account the apriori specified by
       * useAprioriXXX ()
       * @throw MissingVariableInDatabase if a variable of the BN is not found
       * in the database.
       * @throw UnknownLabelInDatabase if a label is found in the databast that
       * do not correpond to the variable.
       */
      BayesNet< GUM_SCALAR > learnParameters(bool take_into_account_score = true);

      /// @return Returns a string representation of this BNLearner's current features.
      std::string toString() const;

      /// @return a representation of the state of the learner in the form vector<key,value,comment>
      std::vector< std::tuple< std::string, std::string, std::string > > state() const;

      private:
      /// read the first line of a file to find column names
      NodeProperty< Sequence< std::string > > _labelsFromBN_(const std::string&            filename,
                                                             const BayesNet< GUM_SCALAR >& src);
    };

    /// Prints BNLearner's current features
    template < typename GUM_SCALAR >
    std::ostream& operator<<(std::ostream& output, const BNLearner< GUM_SCALAR >& learner);
  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/learning/BNLearner_tpl.h>

#endif /* GUM_LEARNING_BN_LEARNER_H */
