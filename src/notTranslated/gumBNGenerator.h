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

#ifndef GUM_BN_GENERATOR_H
#define GUM_BN_GENERATOR_H

#include <string>

#include <agrum/core/gumHashTable.h>
#include <agrum/BN/gumBayesNet.h>
#include <agrum/gumGraph.h>

/**
 * This class generates bayesian networks.
  *
  * The following parameter are allowed:
  *    - DET_RATIO:  The determinism ratio;
  *    - DET_NOISE:  The determinism noise (see method getDeterminismNoise() );
  *    - MIN_DOM_SIZE or MAX_DOM_SIZE:     The minimal and maximal domain size for each variable.
  *    - All parameters allowed for a gumDAGGenerator.
  *
  * The parameters can be given at construction time, through a gumHashTable<string, string>,
  * or after construction through the setConfiguration() method.
  *
  * If some parameters in the gumHashTable are erroneous or missing, the their default
  * values are chosen (which is usually a random value).
  *
  * @see gumDAGGenerator
  *
  */

class gumBNGenerator: public gumDAGGenerator {

  public:
    /* ============================================================================== */
    /* ============================================================================== */
    /* ===                    CONSTRUCTORS AND DESTRUCTOR                         === */
    /* ============================================================================== */
    /* ============================================================================== */

    /**
     * @name Constructors and destructor
     */
    /// @{

    /**
     * Default constructor.
     *
     * All parameters are randomly generated.
     * @param maxNodes The maximum number of nodes allowed.
     * @param maxDomainSize The maximum domain size allowed.
     */
    gumBNGenerator( unsigned int maxNodes = 100, unsigned int maxDomainSize = 4 );

    /**
     * Fast configuration constructor.
     *
     * @param config A hashtable which key's are a parameters and value's are those of
     * their respective parameter
     * value. The list of the parameters is given in the class documentation.
     */
    gumBNGenerator( gumHashTable<std::string, std::string> config );

    /**
     * Destuctor.
     */
    ~gumBNGenerator();

    /// @}

    /* ============================================================================== */
    /* ============================================================================== */
    /* ===                  PARAMETERS GETTERS AND SETTER                         === */
    /* ============================================================================== */
    /* ============================================================================== */

    /**
     * @name Parameters getters and setters
     */
    /// @{

    /**
     * Set the configuration of this generator.
     *
     * A union is made with the current configuration. See the class documentation for
     * a list of the parameters.
     *
     * @param configuration A hastable which key's are parameters and values are those of
     * their respective parameter.
     */
    virtual void setConfiguration( const gumHashTable<std::string, std::string>& configuration );

    /**
     * @return Returns the minimal domain size of a variable.
     */
    unsigned int getMinDomainSize() const;

    /**
     * Set the minimal domain size of a variable.
     */
    void setMinDomainSize( unsigned int n );

    /**
     * @return Returns the maximal domain size of a variable.
     */
    unsigned int getMaxDomainSize() const;

    /**
     * Set the maximal domain size of a variable.
     */
    void setMaxDomainSize( unsigned int n );

    /**
     * @return Returns the ratio of deterministic CPTs (in percents).
     */
    short getDeterminism() const;

    /**
     * Set the ratio of deterministic CPTs (in percents).
     */
    void setDeterminism( short ratio );

    /**
     * The noise added to a deterministic CPT is a floating number which is removed at the
     * "1" value in the CPT, and added to the 0, the value the added is equal to
     * noise / #zeroes.
     *
     * For example if a root node A, with domain type {true, false} and CPT [0,1],
     * then with a noise of 0.001, the CPT value's would be [0.001, 0.999].
     *
     * @return Returns the noise added to deterministic values.
     */
    float getDeterminismNoise() const;

    /**
     * Set the value of noise for deterministic CPTs.
     */
    void setDeterminismNoise( float noise );

    /// @}

    /* ============================================================================== */
    /* ============================================================================== */
    /* ===                          GENERATION METHODS                            === */
    /* ============================================================================== */
    /* ============================================================================== */

    /**
     * @name Generation methods
     */
    /// @{

    /**
     * Generates a bayesian network.
     *
     * @return Returns a randomly generated bayesian network.
     */
    gumBayesNet<float> generateBN() const;

    /**
     * Generates a bayesian network from a given DAG.
     *
     * @param skeleton The DAG used for generating the bayesian network.
     * @return Returns a bayesian network randomly generated.
     */
    gumBayesNet<float> generateCPTs( const gumDAGTpl<gumNode, gumArc>& skeleton ) const;

    /// @}

};

#ifndef GUM_NO_INLINE
#include "gumBNGenerator.inl"
#endif

#endif  // GUM_BN_GENERATOR_H

