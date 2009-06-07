/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
// ============================================================================
#ifndef GUM_BIF_IO_H
#define GUM_BIF_IO_H
// ============================================================================
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
// ============================================================================
#include <agrum/BN/io/BNIO.h>
// ============================================================================
#include <agrum/core/debug.h>
// ============================================================================
#include <agrum/BN/io/BIFparser/BIFDriver.h>
// ============================================================================
namespace gum {
// ============================================================================
/**
 * @class BIFWriter
 * @brief Writes a BayesNet in the BIF format.
 * @ingroup bn_group
 *
 * This class servers to write the content of a Bayesian Network in
 * the BIF format. See
 * http://www.cs.cmu.edu/~fgcozman/Research/InterchangeFormat/Old/xmlbif02.html
 * for information on this format.
 *
 */
template<typename T_DATA>
class BIFWriter: public BNWriter<T_DATA> {
  public:
  // ==========================================================================
  /// @name Constructor & destructor
  // ==========================================================================
  /// @{

    /**
     * Default constructor.
     */
    BIFWriter();

    /**
     * Destructor.
     */
    virtual ~BIFWriter();

  /// @}

    /**
     * Writes a Bayesian Network in the output stream using the BIF format.
     *
     * @param ouput The output stream.
     * @param bn The Bayesian Network writen in output.
     * @throws IOError Raised if and I/O error occurs.
     */
    virtual void write( std::ostream &output, const BayesNet<T_DATA>& bn );

    /**
     * Writes a Bayesian Network in the referenced file using the BIF format.
     * If the files doesn't exists, it is created.
     *
     * @param filePath The path to the file used to write the Bayesian Network.
     * @param bn The Bayesian Network writed in the file.
     * @throws IOError Raised if and I/O error occurs.
     */
    virtual void write( std::string filePath, const BayesNet<T_DATA>& bn );


  private:
    // Returns the header of the BIF file.
    std::string __header( const BayesNet<T_DATA>& bn );

    // Returns a bloc defining a variable in the BIF format.
    std::string __variableBloc( const DiscreteVariable& var );

    // Returns a bloc defining a variable's CPT in the BIF format.
    std::string __variableCPT( const Potential<T_DATA>& cpt );

    // Returns the modalities labels of the variables in varsSeq
    std::string __variablesLabels( const Sequence<const DiscreteVariable*>& varsSeq,
        const Instantiation& inst );
};
// ============================================================================
/**
 * @class
 * @brief Pure virtual class for reading a BN from a file.
 *
 * Every class used to read the content of a Bayesian Network from a
 * stream, or a file must be a subclass of BNReader.
 */
// ============================================================================
class BIFReader: public BNReader {
  public:
  // ==========================================================================
  /// @name Constructor & destructor
  // ==========================================================================
  /// @{

    /**
     * Default constructor.
     */
    BIFReader();

    /**
     * Default destructor.
     */
    virtual ~BIFReader();

  /// @}

    /**
     * Reads a Bayesian Network from the file referenced by filePath into`
     * parameter bayesNet.
     * @return Returns true if the parsing went well.
     */
    virtual bool read( std::string filePath, BayesNet<double>* bayesNet );
};
// ============================================================================
} /* namespace gum */
// ============================================================================
#include <agrum/BN/io/BIFIO.tcc>
// ============================================================================
#endif    // GUM_BIF_IO_H
// ============================================================================
