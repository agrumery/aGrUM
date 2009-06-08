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
/**
 * @file
 * @brief Headers of BIFDriver.
 *
 * @author Lionel Torti
 */
// ============================================================================
#ifndef GUM_BIF_DRIVER_H
#define GUM_BIF_DRIVER_H
// ============================================================================
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
// ============================================================================
#include <agrum/core/hashTable.h>
// ============================================================================
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFactory.h>
// ============================================================================
#include <agrum/BN/io/BIFparser/BIFScanner.h>
#include <agrum/BN/io/BIFparser/generated/BIFParser.hpp>
// ============================================================================
#include <agrum/multidim/labelizedVariable.h>
// ============================================================================
namespace gum_bif {
  /**
   * @class BIFDriver
   * @brief This class is a driver for the BIF parser and scanner.
   */
  class BIFDriver {
    public:
    // =========================================================================
    /// @name Constructors & destructor
    // =========================================================================
      /// @{

      /**
       * Default constructor.
       * @param bayesNet A pointer over the BayesNet used by the factory. It can
       *                 be a non-empty BayesNet.
       */
      BIFDriver(gum::BayesNet<double>* bayesNet);

      /// Destructor.
      ~BIFDriver();

      /// @}
    // =========================================================================
    /// @name Stuff used by the parser
    // =========================================================================
      /// @{

      /**
       * Returns the factory used to create the BayesNet.
       */
      gum::BayesNetFactory<double>& factory();

      /**
       * Returns the lexer.
       */
      BIFScanner& lexer();

      /// @}
    // =========================================================================
    /// @name Getters and setters
    // =========================================================================
      /// @{
      /**
       * Returns the stream name.
       */
      const std::string& streamName() const;

      /**
       * Returns the trace scanning flag.
       */
      bool traceScanning() const;

      /**
       * Sets the trace scanning flat.
       */
      void traceScanning(bool b);

      /**
       * Returns the trace parsing flag.
       */
      bool traceParsing() const;

      /**
       * Sets the trace parsing flag.
       */
      void traceParsing(bool b);

      /// @}
    // ==========================================================================
    ///@name Parse methods.
    // ==========================================================================
      /// @{

      /**
       * Parse a stream using a SkoobParser.
       */
      bool parseStream(std::istream& in,
                       const std::string& sname = "stream input");

      /**
       * Parse a string using a SkoobParser.
       */
      bool parseString(const std::string& input,
                       const std::string& sname = "string stream");

      /**
       * Parse a file using a SkoobScanner.
       */
      bool parseFile(const std::string& filename);

      /**
       * Import the compilation unit in the current PRMFactory.
       */
      void import(const std::string& unitname);

      /// @}
    // ==========================================================================
    /// @name Error handling methods.
    // ==========================================================================
      /// @{

      /**
       * Prints an error.
       */
      void error(const class location& l, const std::string& m);

      /**
       * Prints an error.
       */
      void error(const std::string& m);

      /// @}

    private:
    // ==========================================================================
    /// @name Private members
    // ==========================================================================
      /// @{

      /// The Bayes Net filled by this driver.
      gum::BayesNetFactory<double>* __factory;
      /// The flex lexer
      BIFScanner* __lexer;
      /// The name of the current stream, used for error handling
      std::string __streamName;
      /// Debugging options
      bool __traceScanning;
      /// Debugging options
      bool __traceParsing;

      /// @}
    // ==========================================================================
  };
} /* namespace gum_bif */
// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/BN/io/BIFparser/BIFDriver.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_BIF_DRIVER_H */
// ============================================================================
