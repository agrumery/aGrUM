/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief the base class for estimating parameters of CPTs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    // check the coherency between the parameters passed to setParameters functions
    template < typename GUM_SCALAR >
    void ParamEstimator::_checkParameters_(const NodeId                 target_node,
                                           const std::vector< NodeId >& conditioning_nodes,
                                           Tensor< GUM_SCALAR >&        pot) {
      // check that the nodes passed in arguments correspond to those of pot
      const Sequence< const DiscreteVariable* >& vars = pot.variablesSequence();
      if (vars.size() == 0) { GUM_ERROR(SizeError, "the tensor contains no variable") }

      const auto& database  = counter_.database();
      const auto& node2cols = counter_.nodeId2Columns();
      if (node2cols.empty()) {
        if (database.domainSize(target_node) != vars[0]->domainSize()) {
          GUM_ERROR(SizeError,
                    "Variable " << vars[0]->name() << "of the tensor to be filled "
                                << "has a domain size of " << vars[0]->domainSize()
                                << ", which is different from that of node " << target_node
                                << " which is equal to " << database.domainSize(target_node));
        }
        for (std::size_t i = 1; i < vars.size(); ++i) {
          if (database.domainSize(conditioning_nodes[i - 1]) != vars[i]->domainSize()) {
            GUM_ERROR(SizeError,
                      "Variable " << vars[i]->name() << "of the tensor to be filled "
                                  << "has a domain size of " << vars[i]->domainSize()
                                  << ", which is different from that of node "
                                  << conditioning_nodes[i - 1] << " which is equal to "
                                  << database.domainSize(conditioning_nodes[i - 1]));
          }
        }
      } else {
        std::size_t col = node2cols.second(target_node);
        if (database.domainSize(col) != vars[0]->domainSize()) {
          GUM_ERROR(SizeError,
                    "Variable " << vars[0]->name() << "of the tensor to be filled "
                                << "has a domain size of " << vars[0]->domainSize()
                                << ", which is different from that of node " << target_node
                                << " which is equal to " << database.domainSize(col));
        }
        for (std::size_t i = 1; i < vars.size(); ++i) {
          col = node2cols.second(conditioning_nodes[i - 1]);
          if (database.domainSize(col) != vars[i]->domainSize()) {
            GUM_ERROR(SizeError,
                      "Variable " << vars[i]->name() << "of the tensor to be filled "
                                  << "has a domain size of " << vars[i]->domainSize()
                                  << ", which is different from that of node "
                                  << conditioning_nodes[i - 1] << " which is equal to "
                                  << database.domainSize(col));
          }
        }
      }
    }

    /// sets the CPT's parameters corresponding to a given nodeset
    template < typename GUM_SCALAR >
    INLINE typename std::enable_if< !std::is_same< GUM_SCALAR, double >::value, double >::type
        ParamEstimator::_setParameters_(const NodeId                 target_node,
                                        const std::vector< NodeId >& conditioning_nodes,
                                        Tensor< GUM_SCALAR >&        pot,
                                        const bool                   compute_log_likelihood) {
      _checkParameters_(target_node, conditioning_nodes, pot);

      std::vector< double > params;
      double                log_likelihood = 0.0;
      if (compute_log_likelihood) {
        const auto xparams = parametersAndLogLikelihood(target_node, conditioning_nodes);
        params             = std::move(xparams).first;
        log_likelihood     = xparams.second;
      } else {
        params = parameters(target_node, conditioning_nodes);
      }

      // transform the vector of double into a vector of GUM_SCALAR
      const std::size_t         size = params.size();
      std::vector< GUM_SCALAR > xparams(size);
      for (std::size_t i = std::size_t(0); i < size; ++i)
        xparams[i] = GUM_SCALAR(params[i]);

      pot.fillWith(xparams);
      return log_likelihood;
    }

    /// sets the CPT's parameters corresponding to a given nodeset
    template < typename GUM_SCALAR >
    INLINE typename std::enable_if< std::is_same< GUM_SCALAR, double >::value, double >::type
        ParamEstimator::_setParameters_(const NodeId                 target_node,
                                        const std::vector< NodeId >& conditioning_nodes,
                                        Tensor< GUM_SCALAR >&        pot,
                                        const bool                   compute_log_likelihood) {
      _checkParameters_(target_node, conditioning_nodes, pot);

      std::vector< double > params;
      double                log_likelihood = 0.0;
      if (compute_log_likelihood) {
        const auto xparams = parametersAndLogLikelihood(target_node, conditioning_nodes);
        params             = std::move(xparams).first;
        log_likelihood     = xparams.second;
      } else {
        params = parameters(target_node, conditioning_nodes);
      }

      pot.fillWith(params);
      return log_likelihood;
    }

    /// sets the CPT's parameters corresponding to a given nodeset
    template < typename GUM_SCALAR >
    INLINE double ParamEstimator::setParameters(const NodeId                 target_node,
                                                const std::vector< NodeId >& conditioning_nodes,
                                                Tensor< GUM_SCALAR >&        pot,
                                                const bool compute_log_likelihood) {
      return _setParameters_(target_node, conditioning_nodes, pot, compute_log_likelihood);
    }

    /// assign a new Bayes net to all the counter's generators depending on a BN
    template < typename GUM_SCALAR >
    INLINE void ParamEstimator::setBayesNet(const BayesNet< GUM_SCALAR >& new_bn) {
      counter_.setBayesNet(new_bn);
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
