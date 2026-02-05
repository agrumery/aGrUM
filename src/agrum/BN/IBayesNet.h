/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Class representing the minimal interface for Bayesian network with no numerical data.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Lionel TORTI
 *
 */
#ifndef GUM_SIMPLE_BAYES_NET_H
#define GUM_SIMPLE_BAYES_NET_H

#include <utility>

#include <agrum/agrum.h>

#include <agrum/base/graphicalModels/DAGmodel.h>
#include <agrum/base/multidim/tensor.h>

namespace gum {
  template < typename GUM_SCALAR >
  class BayesNetFactory;

  /**
   * @class IBayesNet
   * @headerfile IBayesNet.h <agrum/BN/IBayesNet.h>
   * @brief Class representing the minimal interface for Bayesian network with no numerical data.
   * @ingroup bn_group
   *
   * This class is used as a base class for different versions of Bayesian
   * Networks. No data (except the dag herited from DAGmodel are included in this class.
   *
   * Many algorithms inference for instance) may use this class when a generic BN is needed.
   */
  template < typename GUM_SCALAR >
  class IBayesNet: public DAGmodel {
    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    IBayesNet();
    explicit IBayesNet(std::string name);

    /**
     * Destructor.
     */
    virtual ~IBayesNet();

    /**
     * Copy constructor.
     */
    IBayesNet(const IBayesNet< GUM_SCALAR >& source);

    /**
     * Copy operator.
     */
    IBayesNet< GUM_SCALAR >& operator=(const IBayesNet< GUM_SCALAR >& source);

    /// @}
    // ===========================================================================
    /// @name Pure Virtual methods
    // ===========================================================================
    /// @{

    /**
     * Returns the CPT of a variable.
     *
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const Tensor< GUM_SCALAR >& cpt(NodeId varId) const = 0;

    /**
     * Returns a constant reference to the VariableNodeMap of thisBN
     */
    virtual const VariableNodeMap& variableNodeMap() const = 0;

    /**
     * Returns a constant reference over a variable given it's node id.
     *
     * @throw NotFound If no variable's id matches varId.
     */
    virtual const DiscreteVariable& variable(NodeId id) const = 0;

    /**
     * Return id node from discrete var pointer.
     *
     * @throw NotFound If no variable matches var.
     */
    virtual NodeId nodeId(const DiscreteVariable& var) const = 0;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual NodeId idFromName(const std::string& name) const = 0;

    /**
     * Getter by name
     *
     * @throw NotFound if no such name exists in the graph.
     */
    virtual const DiscreteVariable& variableFromName(const std::string& name) const = 0;
    /// @}

    // ===========================================================================
    /// @name Joint Probability manipulation methods
    // ===========================================================================
    /// @{

    /// Compute a parameter of the joint probability for the BN (given an
    /// instantiation of the vars)
    /// @warning a variable not present in the instantiation is assumed to be
    /// instantiated to 0.
    GUM_SCALAR jointProbability(const Instantiation& i) const;

    /// Compute a parameter of the log joint probability for the BN (given an
    /// instantiation of the vars)
    /// @warning a variable not present in the instantiation is assumed to be
    /// instantiated to 0.
    GUM_SCALAR log2JointProbability(const Instantiation& i) const;

    ///
    /// @}

    /**
     * Check if the BayesNet is consistent (variables, CPT)
     *
     * @return a list of comments on the consistency of the Bayes Net
     */
    std::vector< std::string > check() const;

    /**
     * This operator compares 2 BNs !
     * @warning To identify nodes between BNs, it is assumed that they share the
     *same name.
     *
     * @return true if the src and this are equal.
     */
    bool operator==(const IBayesNet< GUM_SCALAR >& from) const;

    /// @return Returns false if the src and this are equal.
    bool operator!=(const IBayesNet< GUM_SCALAR >& from) const;

    /**
     * Returns the dimension (the number of free parameters) in this bayes net.
     *
     * \f$ dim(G)=\sum_{i \in nodes} ((r_i-1)\cdot q_i) \f$ where \f$ r_i \f$ is
     * the number of instantiations of node \f$ i \f$ and \f$ q_i \f$ is the number
     * of instantiations of its parents.
     */
    Size dim() const;

    /**
     *
     * @return the biggest domainSize among the variables of *this
     */
    Size maxVarDomainSize() const;

    /**
     *
     * @return the smallest value in the CPTs of *this
     */
    GUM_SCALAR minParam() const;

    /**
     *
     * @return the biggest value in the CPTs of *this
     */
    GUM_SCALAR maxParam() const;

    /**
     *
     * @return the smallest value (not equal to 0) in the CPTs of *this
     * @warning can return 0 if no other value in the CPTs than 0...
     */
    GUM_SCALAR minNonZeroParam() const;

    /**
     *
     * @return the biggest value (not equal to 1) in the CPTs of *this
     * @warning can return one if no other value in the CPTs than one....
     */
    GUM_SCALAR maxNonOneParam() const;

    /// @return Returns a dot representation of this IBayesNet.
    virtual std::string toDot() const;

    /// @return Returns a string representation of this IBayesNet.
    std::string toString() const;

    /**
     * @return a Tensor for a (numerical) discrete variable representing an evidence with a float
     * as observed value
     */
    Tensor< GUM_SCALAR > evEq(const std::string& name, double value) const;

    /**
     * @return a Tensor for a (numerical) discrete variable representing an evidence with a
     * interval of float as observed value
     */
    Tensor< GUM_SCALAR > evIn(const std::string& name, double val1, double val2) const;

    /**
     * @return a Tensor for a (numerical) discrete variable representing an evidence with an
     * observed value less than the parameter
     */
    Tensor< GUM_SCALAR > evLt(const std::string& name, double value) const;


    /**
     * @return a Tensor for a (numerical) discrete variable representing an evidence with an
     * observed value greater than the parameter
     */
    Tensor< GUM_SCALAR > evGt(const std::string& name, double value) const;


    /**
     * @brief compute the (approximated) footprint in memory of the model (the footprints of CPTs)
     * @return the size in bytes
     */
    Size memoryFootprint() const;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class IBayesNet< double >;
#endif


  /// Prints map's DAG in output using the Graphviz-dot format.
  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn);
} /* namespace gum */

#include <agrum/BN/IBayesNet_tpl.h>

#endif /* GUM_SIMPLE_BAYES_NET_H */
