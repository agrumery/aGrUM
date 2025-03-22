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
 * @brief the smooth a priori: adds a weight w to all the counts
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    INLINE
    SmoothingPrior::SmoothingPrior(const DatabaseTable&                    database,
                                   const Bijection< NodeId, std::size_t >& nodeId2columns) :
        Prior(database, nodeId2columns) {
      GUM_CONSTRUCTOR(SmoothingPrior);
    }

    /// copy constructor
    INLINE SmoothingPrior::SmoothingPrior(const SmoothingPrior& from) : Prior(from) {
      GUM_CONS_CPY(SmoothingPrior);
    }

    /// move constructor
    INLINE SmoothingPrior::SmoothingPrior(SmoothingPrior&& from) noexcept : Prior(std::move(from)) {
      GUM_CONS_MOV(SmoothingPrior);
    }

    /// virtual copy constructor
    INLINE SmoothingPrior* SmoothingPrior::clone() const { return new SmoothingPrior(*this); }

    /// destructor
    INLINE SmoothingPrior::~SmoothingPrior() { GUM_DESTRUCTOR(SmoothingPrior); }

    /// copy operator
    INLINE SmoothingPrior& SmoothingPrior::operator=(const SmoothingPrior& from) {
      Prior::operator=(from);
      return *this;
    }

    /// move operator
    INLINE SmoothingPrior& SmoothingPrior::operator=(SmoothingPrior&& from) {
      Prior::operator=(std::move(from));
      return *this;
    }

    /// returns the type of the prior
    INLINE PriorType SmoothingPrior::getType() const { return PriorType::SmoothingPriorType; }

    /// indicates whether the prior is tensorly informative
    INLINE bool SmoothingPrior::isInformative() const { return this->weight_ != 0.0; }

    /// returns the prior vector all the variables in the idset
    INLINE void SmoothingPrior::addJointPseudoCount(const IdCondSet&       idset,
                                                    std::vector< double >& counts) {
      // if the idset is empty or the weight is zero, the prior is also empty
      if (idset.empty() || (this->weight_ == 0.0)) return;

      // otherwise, add the weight to all the cells in the counting vector
      for (auto& count: counts)
        count += this->weight_;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
