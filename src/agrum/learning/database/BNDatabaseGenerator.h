
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief
 *
 * @author Santiago CORTIJO and Pierre-Henri WUILLEMIN
 *
 * Constructor
 * @code
 *  gum::learning::BNDatabaseGenerator< float > dbgen(bn);
 * @endcode
 *    (being "bn" a BayesNet<GUM_SCALAR>)
 *
 * CSV Generation:
 * @code
 *  std::string                        csvFileName="foo.csv"
 *  gum::Size                          nbSamples = 100;
 *  bool                               useLabels = false;
 *  bool                               append = false;
 *  std::string                        csvSeparator(",");
 *  gum::learning::BNDatabaseGenerator< float > dbgen(bn);
 *  dbgen.drawSamples(nbSamples);
 *  dbgen.setRandomVarOrder();
 *  dbgen.toCSV(csvFileName, useLabels, append, csvSeparator);
 * @endcode
 *
 * CSV append:
 * @code
 *  std::string                        csvFileName="foo.csv"
 *  gum::Size                          nbSamples = 100;
 *  bool                               useLabels = false;
 *  bool                               append = true;
 *  std::string                        csvSeparator(":::");
 *  bool                               checkOnAppend = true;
 *  dbgen.drawSamples(nbSamples);
 *  dbgen.setVarOrderFromCSV(csv_file, csvSeparator);
 *  dbgen.toCSV(csv_file, useLabels, append, csvSeparator, checkOnAppend);
 * @endcode
 *
 * DatabaseVectInRam mdoe:
 * @code
 *  gum::learning::DatabaseVectInRAM database =
 *      dbgen.toDatabaseVectInRAM(useLabels);
 * @endcode
 *
 */

#ifndef GUM_BN_DATABASE_GENERATOR
#define GUM_BN_DATABASE_GENERATOR

#include <agrum/BN/BayesNet.h>
#include <agrum/core/progressNotification.h>
#include <agrum/learning/database/databaseTable.h>
#include <fstream>

namespace gum {

  namespace learning {
    template < typename GUM_SCALAR >
    class BNDatabaseGenerator : public ProgressNotifier {
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
      /// using ProgressNotifier as notification
      double drawSamples(Size nbSamples);   // drawSamples

      /// generates csv database according to bn
      void toCSV(const std::string& csvFileURL,
                 bool               useLabels = true,
                 bool               append = false,
                 std::string        csvSeparator = ",",
                 bool               checkOnAppend = false) const;

      /// generates a DatabaseVectInRAM
      DatabaseTable<> toDatabaseTable(bool useLabels = true) const;

      /// generates database according to bn into a std::vector
      /// @warning: makes a copy of the whole database
      std::vector< std::vector< Idx > > database() const;

      /// change columns order
      void setVarOrder(const std::vector< Idx >& varOrder);

      /// change columns order using variable names
      void setVarOrder(const std::vector< std::string >& varOrder);

      /// change columns order according to a csv file
      void setVarOrderFromCSV(const std::string& csvFileURL,
                              const std::string& csvSeparator = ",");

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
      const BayesNet< GUM_SCALAR >& __bn;

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


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class BNDatabaseGenerator< double >;
#endif

  } /* namespace learning */
} /* namespace gum */

#include <agrum/learning/database/BNDatabaseGenerator_tpl.h>
#endif /* BN_DATABASE_GENERTOR */
