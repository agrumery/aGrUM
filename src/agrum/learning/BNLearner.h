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
 * @brief A basic pack of learning algorithms that can easily be used
 *
 * The pack currently contains K2, GreedyHillClimbing and LocalSearchWithTabuList
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_BN_LEARNER_H
#define GUM_LEARNING_BN_LEARNER_H

#include <sstream>

#include <agrum/config.h>
#include <agrum/core/bijection.h>
#include <agrum/core/sequence.h>
#include <agrum/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>

#include <agrum/learning/BNLearnUtils/genericBNLearner.h>

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
    template <typename GUM_SCALAR> class BNLearner : public genericBNLearner {

      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /**
       * read the database file for the score / parameter estimation and var names
       */
      BNLearner(const std::string &filename);

      /**
       * read the database file for the score / parameter estimation and var names
       * @param modalities indicate for some nodes (not necessarily all the
       * nodes of the BN) which modalities they should have and in which
       * order these modalities should be stored into the nodes. For instance,
       * if modalities = { 1 -> {True, False, Big} }, then the node of id 1
       * in the BN will have 3 modalities, the first one being True, the
       * second one being False, and the third bein Big.
       * @param parse_database if true, the modalities specified by the user
       * will be considered as a superset of the modalities of the variables. A
       * parsing of the database will allow to determine which ones are really
       * necessary and will keep them in the order specified by the user
       * (NodeProperty modalities). If parse_database is set to false (the
       * default), then the modalities specified by the user will be considered as
       * being exactly those of the variables of the BN (as a consequence, if we
       * find other values in the database, an exception will be raised
       * during learning). */
      BNLearner(const std::string &filename,
                const NodeProperty<Sequence<std::string>> &modalities,
                bool parse_database = false);

      /**
       * wrapper for BNLearner (filename,modalities,parse_database) using a bn to
       *find those
       * modalities.
       **/
      BNLearner(const std::string &filename, const gum::BayesNet<GUM_SCALAR> &src,
                bool parse_database = false);

      /// copy constructor
      BNLearner(const BNLearner &);

      /// move constructor
      BNLearner(BNLearner &&);

      /// destructor
      virtual ~BNLearner();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      BNLearner &operator=(const BNLearner &);

      /// move operator
      BNLearner &operator=(BNLearner &&);

      /// @}

      /// learn a Bayes Net from a file (must have read the db before)
      BayesNet<GUM_SCALAR> learnBN();

      /// learns a BN (its parameters) when its structure is known
      /** @param dag the structure of the Bayesian network
       * @param take_into_account_score The dag passed in argument may have been
       * learnt from a structure learning. In this case, if the score used to
       * learn the structure has an implicit apriori (like K2 which has a
       * 1-smoothing apriori), it is important to also take into account this
       * implicit apriori for parameter learning. By default, if a score exists,
       * we will learn parameters by taking into account the apriori specified by
       * methods useAprioriXXX () + the implicit apriori of the score, else we
       * just take into account the apriori specified by useAprioriXXX () */
      BayesNet<GUM_SCALAR> learnParameters(const DAG &dag,
                                           bool take_into_account_score = true);

      /// learns a BN (its parameters) when its structure is known
      /** @param dag the structure of the Bayesian network
       * @param take_into_account_score The dag passed in argument may have been
       * learnt from a structure learning. In this case, if the score used to
       * learn the structure has an implicit apriori (like K2 which has a
       * 1-smoothing apriori), it is important to also take into account this
       * implicit apriori for parameter learning. By default, if a score exists,
       * we will learn parameters by taking into account the apriori specified by
       * methods useAprioriXXX () + the implicit apriori of the score, else we
       * just take into account the apriori specified by useAprioriXXX ()
       * @throw MissingVariableInDatabase if a variable of the BN is not found in
       * the database.
       * @throw UnknownLabelInDatabase if a label is found in the databast that
       * do not correpond to the variable.
       */
      BayesNet<GUM_SCALAR> learnParameters(const BayesNet<GUM_SCALAR> &bn,
                                           bool take_into_account_score = true);

      private:
      /// read the first line of a file to find column names
      NodeProperty<Sequence<std::string>>
      __labelsFromBN(const std::string &filename, const BayesNet<GUM_SCALAR> &src);
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/learning/BNLearner.tcc>

#endif /* GUM_LEARNING_BN_LEARNER_H */
