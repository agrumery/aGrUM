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
 * @brief
 *
 * @author Santiago CORTIJO
 *
 * Constructor
 *  gum::learning::BNDatabaseGenerator< float > dbgen(bn);
 *    (being "bn" a BayesNet<GUM_SCALAR>)
 *
 * CSV Generation:
 *  std::string                        csvFileName="foo.csv"
 *  gum::Size                          nbSamples = 100;
 *  bool                               useLabels = false;
 *  bool                               append = false;
 *  std::string                        csvSeparator(",");
 *  gum::learning::BNDatabaseGenerator< float > dbgen(bn);
 *  dbgen.drawSamples(nbSamples);
 *  dbgen.setRandomVarOrder();
 *  dbgen.toCSV(csvFileName, useLabels, append, csvSeparator);
 *
 * CSV append:
 *  std::string                        csvFileName="foo.csv"
 *  gum::Size                          nbSamples = 100;
 *  bool                               useLabels = false;
 *  bool                               append = true;
 *  std::string                        csvSeparator(":::");
 *  bool                               checkOnAppend = true;
 *  dbgen.drawSamples(nbSamples);
 *  dbgen.setVarOrderFromCSV(csv_file, csvSeparator);
 *  dbgen.toCSV(csv_file, useLabels, append, csvSeparator, checkOnAppend);
 *
 * DatabaseVectInRam mdoe:
 *  gum::learning::DatabaseVectInRAM database =
 *      dbgen.toDatabaseVectInRAM(useLabels);
 *
 */

#ifndef GUM_BN_DATABASE_GENERATOR
#define GUM_BN_DATABASE_GENERATOR

#include <agrum/BN/BayesNet.h>
#include <agrum/learning/database/databaseVectInRAM.h>
#include <fstream>

namespace gum {

  namespace learning {

    template < typename GUM_SCALAR >
    class BNDatabaseGenerator {

      public:
      // #######################################################################
      /// @name Constructors / Destructors
      // #######################################################################
      /// @{

      /// default constructor
      BNDatabaseGenerator(const BayesNet< GUM_SCALAR >& bn);

      /// destructor
      ~BNDatabaseGenerator();

      /// @}

      // #######################################################################
      /// @name Accessors / Modifiers
      // #######################################################################
      /// @{


      /// generate and stock database, returns log2likelihood
      double drawSamples(Size nbSamples);  // drawSamples

      /// generates csv database according to bn
      void toCSV(const std::string& csvFileURL,
                 bool               useLabels = true,
                 bool               append = false,
                 std::string        csvSeparator = ",",
                 bool               checkOnAppend = false) const;

      /// generates a DatabaseVectInRAM
      DatabaseVectInRAM toDatabaseVectInRAM(bool useLabels = true) const;

      /// generates database according to bn into a std::vector
      /// @warning: makes a copy of the whole database
      std::vector< std::vector< Idx > > database() const;

      /// change columns order
      void setVarOrder(const std::vector< Idx >& varOrder);

      /// change columns order using variable names
      void setVarOrder(const std::vector< std::string >& varOrder);

      /// change columns order according to a csv file
      void setVarOrderFromCSV(const std::string& csvFileURL,
                              const std::string& csvSeparator);

      /// set columns in topoligical order
      void setTopologicalVarOrder();

      /// set columns in antiTopoligical order
      void setAntiTopologicalVarOrder();

      /// set columns in random order
      void setRandomVarOrder();

      /// returns variable order indexes
      std::vector< Idx > varOrder() const;

      /// returns variable order.
      std::vector< std::string > varOrderNames() const;

      /// returns log2Likelihood of generated samples
      double log2likelihood() const;

      /// @}

      private:
      /// Bayesian network
      BayesNet< GUM_SCALAR > __bn;

      /// bijection nodes names
      Bijection< std::string, NodeId > __names2ids;

      /// number of variables
      Size __nbVars;

      /// generated database
      std::vector< std::vector< Idx > > __database;

      /// variable order in generated database
      std::vector< Idx > __varOrder;

      /// whether drawSamples has been already called.
      bool __drawnSamples = false;

      /// log2Likelihood of generated samples
      double __log2likelihood = 0;

      /// returns varOrder from a csv file
      std::vector< Idx >
      __varOrderFromCSV(const std::string& csvFileURL,
                        const std::string& csvSeparator = ",") const;

      /// returns varOrder from a csv file
      std::vector< Idx >
      __varOrderFromCSV(std::ifstream&     csvFile,
                        const std::string& csvSeparator = ",") const;

      // forbidden copies / moves
      BNDatabaseGenerator(const BNDatabaseGenerator&) = delete;
      BNDatabaseGenerator(BNDatabaseGenerator&&) = delete;
      BNDatabaseGenerator& operator=(const BNDatabaseGenerator&) = delete;
      BNDatabaseGenerator& operator=(BNDatabaseGenerator&&) = delete;
    };

    extern template class BNDatabaseGenerator< float >;
    extern template class BNDatabaseGenerator< double >;

  } /* namespace learning */
} /* namespace gum */

#include <agrum/learning/database/BNDatabaseGenerator_tpl.h>
#endif /* BN_DATABASE_GENERTOR */
