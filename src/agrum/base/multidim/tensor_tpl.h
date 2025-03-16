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





/**
 * @file
 * @brief Implementation of the Tensor class.
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <algorithm>

#include <agrum/agrum.h>

#include <agrum/base/multidim/tensor.h>

#include <agrum/base/core/math/math_utils.h>

namespace gum {

  // Default constructor: creates an empty null dimensional matrix
  // choose a MultiDimArray<> as decorated implementation
  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >::Tensor() :
      MultiDimDecorator< GUM_SCALAR >(new MultiDimArray< GUM_SCALAR >(), GUM_SCALAR(1)) {
    GUM_CONSTRUCTOR(Tensor)
  }

  // constructor using aContent as content
  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >::Tensor(MultiDimImplementation< GUM_SCALAR >* aContent) :
      MultiDimDecorator< GUM_SCALAR >(aContent, GUM_SCALAR(1)) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Tensor)
  }

  // copy constructor
  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >::Tensor(const Tensor< GUM_SCALAR >& src) :
      Tensor< GUM_SCALAR >(
          static_cast< MultiDimImplementation< GUM_SCALAR >* >(src.content()->newFactory()),
          *(src.content())) {
    this->empty_value_ = src.empty_value_;
    // GUM_CONS_CPY not here because in called Tensor
    // GUM_CONS_CPY( Tensor );
  }

  /// move constructor
  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >::Tensor(Tensor< GUM_SCALAR >&& from) :
      MultiDimDecorator< GUM_SCALAR >(std::forward< MultiDimDecorator< GUM_SCALAR > >(from)) {
    GUM_CONS_MOV(Tensor)
  }

  // complex copy constructor : we choose the implementation
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >::Tensor(MultiDimImplementation< GUM_SCALAR >*  aContent,
                                     const MultiDimContainer< GUM_SCALAR >& src) :
      MultiDimDecorator< GUM_SCALAR >(aContent) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Tensor)

    if (!src.empty()) {
      this->beginMultipleChanges();

      for (Idx i = 0; i < src.variablesSequence().size(); i++) {
        this->add(*(src.variablesSequence()[i]));
      }

      this->endMultipleChanges();
      this->content()->copyFrom(*src.content());
    }
  }

  // operator= (copy)
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator=(const Tensor< GUM_SCALAR >& src) {
    GUM_OP_CPY(Tensor)
    if (&src == this) return *this;
    MultiDimDecorator< GUM_SCALAR >::operator=(src);
    return *this;
  }

  // operator= (move)
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator=(Tensor< GUM_SCALAR >&& src) {
    GUM_OP_MOV(Tensor)
    if (&src == this) return *this;
    MultiDimDecorator< GUM_SCALAR >::operator=(
        std::forward< MultiDimDecorator< GUM_SCALAR > >(src));
    return *this;
  }

  // destructor

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >::~Tensor() {
    // for debugging purposes
    GUM_DESTRUCTOR(Tensor)
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >* Tensor< GUM_SCALAR >::newFactory() const {
    return new Tensor< GUM_SCALAR >(
        static_cast< MultiDimImplementation< GUM_SCALAR >* >(this->content()->newFactory()));
  }

  // sum of all elements in this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Tensor< GUM_SCALAR >::sum() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return this->empty_value_;
    }
    return gum::projectSum(*this->content());
  }

  // product of all elements in this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Tensor< GUM_SCALAR >::product() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return this->empty_value_;
    }
    return gum::projectProduct(*this->content());
  }

  // max of all elements in this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Tensor< GUM_SCALAR >::max() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return this->empty_value_;
    }
    return gum::projectMax(*this->content());
  }

  // min of all elements in this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Tensor< GUM_SCALAR >::min() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return this->empty_value_;
    }
    return gum::projectMin(*this->content());
  }

  // max of all non-one elements in this
  // warning can return 1 if no other value than 1 ...
  template < typename GUM_SCALAR >
  GUM_SCALAR Tensor< GUM_SCALAR >::maxNonOne() const {
    GUM_SCALAR res;

    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      res = this->empty_value_;
    } else {
      res = this->reduce(
          [](GUM_SCALAR z, GUM_SCALAR p) {
            return (p == static_cast< GUM_SCALAR >(1)) ? z
                 : (z == static_cast< GUM_SCALAR >(1)) ? p
                                                       : (p > z ? p : z);
          },
          static_cast< GUM_SCALAR >(1));
    }

    return res;
  }

  // min of all non-zero elements in this
  // warning can return 0 if no other value than 0 ...
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Tensor< GUM_SCALAR >::minNonZero() const {
    GUM_SCALAR res;

    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      res = this->empty_value_;
    } else {
      res = this->reduce(
          [](GUM_SCALAR z, GUM_SCALAR p) {
            return (p == static_cast< GUM_SCALAR >(0)) ? z
                 : (z == static_cast< GUM_SCALAR >(0)) ? p
                                                       : (p < z ? p : z);
          },
          static_cast< GUM_SCALAR >(0));
    }
    return res;
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR Tensor< GUM_SCALAR >::expectedValue(
      std::function< GUM_SCALAR(const gum::Instantiation&) > f) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return static_cast< GUM_SCALAR >(0);
    }

    GUM_SCALAR res = 0;
    auto       i   = Instantiation(*this);
    for (i.setFirst(); !i.end(); i.inc()) {
      const GUM_SCALAR v_f = f(i);
      if (v_f != GUM_SCALAR(0.0)) res += this->get(i) * v_f;
    }
    return res;
  }

  // entropy of this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Tensor< GUM_SCALAR >::entropy() const {
    return -this->expectedValue([this](const gum::Instantiation& i) -> GUM_SCALAR {
      return GUM_SCALAR(GUM_LOG2_OR_0(this->get(i)));
    });
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >&
               Tensor< GUM_SCALAR >::fillWith(const std::vector< GUM_SCALAR >& data) const {
    this->populate(data);
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >&
               Tensor< GUM_SCALAR >::fillWith(const GUM_SCALAR& val) const {
    this->fill(val);
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >&
               Tensor< GUM_SCALAR >::fillWith(const Tensor< GUM_SCALAR >& src) const {
    if (src.domainSize() != this->domainSize()) {
      GUM_ERROR(InvalidArgument, "Tensor to copy has not the same domainSize.")
    }
    gum::Set< std::string > son;   // set of names
    for (const auto& v: src.variablesSequence()) {
      son.insert(v->name());
    }
    for (const auto& v: this->variablesSequence()) {
      if (!son.contains(v->name())) {
        GUM_ERROR(InvalidArgument,
                  "Variable <" << v->name() << "> not present in src (" << son << ").")
      }
      // we check size, labels and order of labels in the same time
      if (v->toString() != src.variable(v->name()).toString()) {
        GUM_ERROR(InvalidArgument, "Variables <" << v->name() << "> are not identical.")
      }
    }

    Instantiation Isrc(src);
    Instantiation Idst(*this);
    for (Isrc.setFirst(); !Isrc.end(); ++Isrc) {
      for (Idx i = 0; i < this->nbrDim(); i++) {
        Idst.chgVal(Isrc.variable(i).name(), Isrc.val(i));
      }
      this->set(Idst, src.get(Isrc));
    }

    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >&
               Tensor< GUM_SCALAR >::fillWith(const Tensor< GUM_SCALAR >&    src,
                                        const std::vector< std::string >& mapSrc) const {
    if (src.nbrDim() != this->nbrDim()) {
      GUM_ERROR(InvalidArgument, "Tensor to copy has not the same size.")
    }
    if (src.nbrDim() != mapSrc.size()) {
      GUM_ERROR(InvalidArgument, "Tensor and vector have not the same size.")
    }
    Instantiation Isrc;
    for (Idx i = 0; i < src.nbrDim(); i++) {
      if (src.variable(mapSrc[i]).domainSize() != this->variable(i).domainSize()) {
        GUM_ERROR(InvalidArgument,
                  "Variables " << mapSrc[i] << " (in the argument) and " << this->variable(i).name()
                               << " have not the same dimension.")
      } else {
        Isrc.add(src.variable(mapSrc[i]));
      }
    }
    Instantiation Idst(*this);
    for (Isrc.setFirst(); !Isrc.end(); ++Isrc, ++Idst) {
      this->set(Idst, src.get(Isrc));
    }

    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::sq() const {
    this->apply([](GUM_SCALAR x) { return x * x; });
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::log2() const {
    this->apply([](GUM_SCALAR x) { return std::log2(x); });
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::sgn() const {
    this->apply([](GUM_SCALAR x) { return (GUM_SCALAR(0) < x) - (x < GUM_SCALAR(0)); });
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Tensor< GUM_SCALAR >::KL(const Tensor< GUM_SCALAR >& p) const {
    if (this->nbrDim() != p.nbrDim())
      GUM_ERROR(InvalidArgument,
                "BNdistance between tensors with different numbers of dimensions")
    for (const auto var: p.variablesSequence()) {
      if (!this->contains(*var))
        GUM_ERROR(InvalidArgument, "A variable in the argument does not belong to the tensor.")
    }
    for (const auto var: this->variablesSequence()) {
      if (!p.contains(*var))
        GUM_ERROR(InvalidArgument, "A variable does not belong to the argument.")
    }

    Instantiation inst(*this);
    auto          res = static_cast< GUM_SCALAR >(0);
    for (inst.setFirst(); !inst.end(); inst.inc()) {
      GUM_SCALAR x = this->get(inst);
      GUM_SCALAR y = p.get(inst);
      if (static_cast< GUM_SCALAR >(0) == x)   // 0*log(0/y)=0
        continue;

      if (static_cast< GUM_SCALAR >(0) == y)
        // we know that x!=0;
        GUM_ERROR(FatalError, "The argument has a 0 at " << inst << " while the tensor has not.")

      res += x * std::log2(x / y);
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::abs() const {
    this->apply([](GUM_SCALAR x) {
      if (x >= 0) return x;
      else return -x;
    });
    return *this;
  }

  // normalisation of this
  // do nothing is sum is 0
  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::normalize() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      if (this->empty_value_ != static_cast< GUM_SCALAR >(0))
        this->empty_value_ = static_cast< GUM_SCALAR >(1.0);
    } else {
      GUM_SCALAR s = sum();

      if (s != (GUM_SCALAR)0) {
        this->apply([s](GUM_SCALAR x) { return x / s; });
      }
    }
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >&
               Tensor< GUM_SCALAR >::normalizeAsCPT(const Idx& varId) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      if (this->empty_value_ != static_cast< GUM_SCALAR >(0)) {
        this->empty_value_ = static_cast< GUM_SCALAR >(1.0);
      } else {
        GUM_ERROR(FatalError, "Normalization for a tensor that sum to 0 in " << *this)
      }
    } else {
      if (varId >= this->nbrDim()) {
        GUM_ERROR(FatalError, varId << " is not a position for " << *this)
      }
      Instantiation inst(*this);
      const auto&   v = this->variable(varId);

      for (inst.setFirst(); !inst.end(); inst.incNotVar(v)) {
        auto s = (GUM_SCALAR)0.0;
        for (inst.setFirstVar(v); !inst.end(); inst.incVar(v))
          s += this->get(inst);
        if (s == (GUM_SCALAR)0.0) {
          GUM_ERROR(FatalError, "Normalization for a tensor that sum to 0 in " << *this)
        }
        if (s != (GUM_SCALAR)1.0) {
          for (inst.setFirstVar(v); !inst.end(); inst.incVar(v))
            this->set(inst, this->get(inst) / s);
        }
        inst.setFirstVar(v);   // to remove inst.end()
      }
    }
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::scale(GUM_SCALAR v) const {
    this->apply([v](GUM_SCALAR x) { return x * v; });
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::translate(GUM_SCALAR v) const {
    this->apply([v](GUM_SCALAR x) { return x + v; });
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::inverse() const {
    this->apply([](GUM_SCALAR x) { return 1 / x; });
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >
         Tensor< GUM_SCALAR >::sumOut(const gum::VariableSet& del_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return Tensor< GUM_SCALAR >().fillWith(this->empty_value_);
    }

    // if we remove all the variables, create an empty tensor
    // TODO: remove this test when operations will be able to handle empty tensors
    if (this->variablesSequence().size() <= del_vars.size()) {
      bool equal = true;
      for (const auto var: this->variablesSequence()) {
        if (!del_vars.exists(var)) {
          equal = false;
          break;
        }
      }
      if (equal) { return Tensor< GUM_SCALAR >().fillWith(this->sum()); }
    }

    return Tensor< GUM_SCALAR >(gum::projectSum(*this->content(), del_vars));
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >
         Tensor< GUM_SCALAR >::prodOut(const gum::VariableSet& del_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return Tensor< GUM_SCALAR >().fillWith(this->empty_value_);
    }

    // if we remove all the variables, create an empty tensor
    // TODO: remove this test when operations will be able to handle empty tensors
    if (this->variablesSequence().size() <= del_vars.size()) {
      bool equal = true;
      for (const auto var: this->variablesSequence()) {
        if (!del_vars.exists(var)) {
          equal = false;
          break;
        }
      }
      if (equal) { return Tensor< GUM_SCALAR >().fillWith(this->product()); }
    }

    return Tensor< GUM_SCALAR >(gum::projectProduct(*this->content(), del_vars));
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >
         Tensor< GUM_SCALAR >::minOut(const gum::VariableSet& del_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return Tensor< GUM_SCALAR >().fillWith(this->empty_value_);
    }

    // if we remove all the variables, create an empty tensor
    // TODO: remove this test when operations will be able to handle empty tensors
    if (this->variablesSequence().size() <= del_vars.size()) {
      bool equal = true;
      for (const auto var: this->variablesSequence()) {
        if (!del_vars.exists(var)) {
          equal = false;
          break;
        }
      }
      if (equal) { return Tensor< GUM_SCALAR >().fillWith(this->min()); }
    }

    return Tensor< GUM_SCALAR >(gum::projectMin(*this->content(), del_vars));
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >
         Tensor< GUM_SCALAR >::maxOut(const gum::VariableSet& del_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return Tensor< GUM_SCALAR >().fillWith(this->empty_value_);
    }

    // if we remove all the variables, create an empty tensor
    // TODO: remove this test when operations will be able to handle empty tensors
    if (this->variablesSequence().size() <= del_vars.size()) {
      bool equal = true;
      for (const auto var: this->variablesSequence()) {
        if (!del_vars.exists(var)) {
          equal = false;
          break;
        }
      }
      if (equal) { return Tensor< GUM_SCALAR >().fillWith(this->max()); }
    }

    return Tensor< GUM_SCALAR >(gum::projectMax(*this->content(), del_vars));
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >
         Tensor< GUM_SCALAR >::sumIn(const gum::VariableSet& kept_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return Tensor< GUM_SCALAR >().fillWith(this->empty_value_);
    }

    // if kept_var is empty, create an empty tensor
    // TODO: remove this test when operations will be able to handle empty tensors
    if (kept_vars.empty()) { return Tensor< GUM_SCALAR >().fillWith(this->sum()); }

    return Tensor< GUM_SCALAR >(gum::projectSum(*this->content(), complementVars_(kept_vars)));
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >
         Tensor< GUM_SCALAR >::prodIn(const gum::VariableSet& kept_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return Tensor< GUM_SCALAR >().fillWith(this->empty_value_);
    }

    // if kept_var is empty, create an empty tensor
    // TODO: remove this test when operations will be able to handle empty tensors
    if (kept_vars.empty()) { return Tensor< GUM_SCALAR >().fillWith(this->product()); }

    return Tensor< GUM_SCALAR >(
        gum::projectProduct(*this->content(), complementVars_(kept_vars)));
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >
         Tensor< GUM_SCALAR >::minIn(const gum::VariableSet& kept_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return Tensor< GUM_SCALAR >().fillWith(this->empty_value_);
    }

    // if kept_var is empty, create an empty tensor
    // TODO: remove this test when operations will be able to handle empty tensors
    if (kept_vars.empty()) { return Tensor< GUM_SCALAR >().fillWith(this->min()); }

    return Tensor< GUM_SCALAR >(gum::projectMin(*this->content(), complementVars_(kept_vars)));
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR >
         Tensor< GUM_SCALAR >::maxIn(const gum::VariableSet& kept_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) {
      return Tensor< GUM_SCALAR >().fillWith(this->empty_value_);
    }

    // if kept_var is empty, create an empty tensor
    // TODO: remove this test when operations will be able to handle empty tensors
    if (kept_vars.empty()) { return Tensor< GUM_SCALAR >().fillWith(this->max()); }

    return Tensor< GUM_SCALAR >(gum::projectMax(*this->content(), complementVars_(kept_vars)));
  }

  template < typename GUM_SCALAR >
  INLINE Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::isNonZeroMap() const {
    auto p = Tensor< GUM_SCALAR >(*this);
    p.apply([](GUM_SCALAR x) {
      if (x != static_cast< GUM_SCALAR >(0)) return static_cast< GUM_SCALAR >(1);
      else return static_cast< GUM_SCALAR >(0);
    });
    return p;
  }

  template < typename GUM_SCALAR >
  gum::VariableSet Tensor< GUM_SCALAR >::complementVars_(const gum::VariableSet& vars) const {
    gum::VariableSet cplt;

    for (const auto x: this->variablesSequence())
      if (!vars.contains(x)) cplt.insert(x);

    return cplt;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::reorganize(
      const std::vector< const DiscreteVariable* >& vars) const {
    if (vars.size() != this->nbrDim())
      GUM_ERROR(InvalidArgument,
                "The argument contains " << vars.size() << " variables instead of "
                                         << this->nbrDim() << ".")
    for (const auto var: vars) {
      if (!this->contains(*var))
        GUM_ERROR(InvalidArgument, "A variable in the argument does not belong to the tensor.")
    }

    Tensor< GUM_SCALAR > p;
    p.beginMultipleChanges();
    for (const auto var: vars)
      p.add(*var);
    p.endMultipleChanges();
    p.copyFrom(*this, nullptr);   // copy *this in p using the same order

    return p;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >
      Tensor< GUM_SCALAR >::reorganize(const std::vector< std::string >& vars) const {
    std::vector< const DiscreteVariable* > res;

    gum::HashTable< std::string, const gum::DiscreteVariable* > namesToVars;
    for (gum::Idx i = 0; i < this->nbrDim(); i++)
      namesToVars.insert(this->variable(i).name(), &(this->variable(i)));

    for (const auto& name: vars) {
      if (!namesToVars.exists(name)) {
        GUM_ERROR(gum::InvalidArgument,
                  "'" << name << "' is a not a name of a variable in this tensor")
      }
      res.push_back(namesToVars[name]);
    }
    return reorganize(res);
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::putFirst(const DiscreteVariable* var) const {
    if (!this->contains(*var)) {
      GUM_ERROR(InvalidArgument, "The variable to put first does not belong to the tensor")
    }
    if (&(this->variable(0)) == var) return Tensor< GUM_SCALAR >(*this);

    std::vector< const DiscreteVariable* > vars;
    vars.push_back(var);
    for (Idx i = 0; i < this->nbrDim(); i++)
      if (&(this->variable(i)) != var) vars.push_back(&(this->variable(i)));

    return this->reorganize(vars);
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::putFirst(const std::string& varname) const {
    const DiscreteVariable* var = nullptr;

    for (gum::Idx i = 0; i < this->nbrDim(); i++)
      if (this->variable(i).name() == varname) {
        var = &(this->variable(i));
        break;
      }
    if (var == nullptr)
      GUM_ERROR(InvalidArgument,
                "The variable '" << varname << "' to put first does not belong to the tensor")
    return this->putFirst(var);
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::extract(const Instantiation& inst) const {
    Tensor< GUM_SCALAR > p;
    p.extractFrom(*this, inst);

    return p;
  }

  template < typename GUM_SCALAR >
  Idx Tensor< GUM_SCALAR >::draw() const {
    if (this->nbrDim() != 1) {
      GUM_ERROR(FatalError, "To draw from a tensor, the dimension must be 1")
    }

    auto          r = static_cast< GUM_SCALAR >(randomProba());
    Instantiation Ip(*this);
    for (Ip.setFirst(); !Ip.end(); Ip.inc()) {
      r -= this->get(Ip);
      if (r <= 0) return Ip.val(0);
    }
    return this->variable(0).domainSize() - 1;
  }

  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream& out, const Tensor< GUM_SCALAR >& array) {
    out << array.toString();
    return out;
  }

  // argmax of all elements in this
  template < typename GUM_SCALAR >
  Set< Instantiation > Tensor< GUM_SCALAR >::findAll(GUM_SCALAR v) const {
    Instantiation        I(*this);
    Set< Instantiation > res;

    //    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->content_)->empty()) { return
    //    res; }
    for (I.setFirst(); !I.end(); ++I) {
      if (this->get(I) == v) res.insert(Instantiation(I, false));
    }
    return res;
  }

  // argmax of all elements in this
  template < typename GUM_SCALAR >
  INLINE std::pair< Set< Instantiation >, GUM_SCALAR > Tensor< GUM_SCALAR >::argmax() const {
    auto m = max();
    return std::pair(findAll(m), m);
  }

  // argmin of all elements in this
  template < typename GUM_SCALAR >
  INLINE std::pair< Set< Instantiation >, GUM_SCALAR > Tensor< GUM_SCALAR >::argmin() const {
    auto m = min();
    return std::pair(findAll(m), m);
  }

  template < typename GUM_SCALAR >
  const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::random() const {
    if (this->domainSize() == 0) return *this;

    std::vector< GUM_SCALAR > v;
    v.reserve(this->domainSize());
    for (Size i = 0; i < this->domainSize(); ++i) {
      auto r = (GUM_SCALAR)randomProba();
      v.push_back(r);
    }
    this->fillWith(v);
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::randomDistribution() const {
    if (this->domainSize() == 0) {
      this->fillWith((GUM_SCALAR)1.0);
    } else {
      this->fillWith(gum::randomDistribution< GUM_SCALAR >(this->domainSize()));
    }

    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::randomCPT() const {
    if (this->domainSize() == 0) {
      this->fillWith((GUM_SCALAR)1.0);
    } else {
      gum::Instantiation I(*this);
      const auto&        v = this->variable(0);
      for (I.setFirstNotVar(v); !I.end(); I.incNotVar(v)) {
        const auto& distrib = gum::randomDistribution< GUM_SCALAR >(v.domainSize());
        for (I.setFirstVar(v); !I.end(); I.incVar(v)) {
          this->set(I, distrib[I.val(0)]);
        }
        I.unsetEnd();
      }
    }
    return *this;
  }

  template < typename GUM_SCALAR >
  const Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::noising(GUM_SCALAR alpha) const {
    if ((alpha < GUM_SCALAR(0.0)) || (alpha > GUM_SCALAR(1.0))) {
      GUM_ERROR(InvalidArgument, "alpha must be in [0,1]")
    }
    Tensor< GUM_SCALAR > noise(*this);
    return fillWith(scale(1 - alpha) + noise.randomCPT().scale(alpha)).normalizeAsCPT();
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::new_abs() const {
    return Tensor< GUM_SCALAR >(*this).abs();
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::new_sq() const {
    return Tensor< GUM_SCALAR >(*this).sq();
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::new_log2() const {
    return Tensor< GUM_SCALAR >(*this).log2();
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::new_sgn() const {
    return Tensor< GUM_SCALAR >(*this).sgn();
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator<<(const DiscreteVariable& v) {
    this->add(v);
    return *this;
  }

  /// the function to be used to add two Tensors
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >
      Tensor< GUM_SCALAR >::operator+(const Tensor< GUM_SCALAR >& p2) const {
    if (p2.empty()) return Tensor< GUM_SCALAR >(*this).translate(p2.empty_value_);
    if (this->empty()) return Tensor< GUM_SCALAR >(p2).translate(this->empty_value_);

    return Tensor< GUM_SCALAR >(*this->content() + *p2.content());
  }

  /// the function to be used to add a GUM_SCALAR to a Tensor
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::operator+(const GUM_SCALAR& v) const {
    return Tensor< GUM_SCALAR >(*this).translate(v);
  }

  /// is an evidence ? (marginal-like but has not to sum to 1)
  template < typename GUM_SCALAR >
  bool Tensor< GUM_SCALAR >::isEvidence() const {
    if (this->nbrDim() != 1) return false;
    if (this->sum() <= 0.0) return false;
    return (this->min() >= 0.0) && (this->max() <= 1.0);
  }

  // max function between two evidence
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >
      Tensor< GUM_SCALAR >::operator|(const Tensor< GUM_SCALAR >& p2) const {
    if (!isEvidence() || !p2.isEvidence())
      GUM_ERROR(InvalidArgument, "The tensors are not evidences.")
    if (this->variable(0) != p2.variable(0))
      GUM_ERROR(InvalidArgument, "The evidence are not on the same variable.")
    Tensor< GUM_SCALAR > res(*this);
    gum::Instantiation      I(res);
    for (I.setFirst(); !I.end(); ++I) {
      res.set(I, std::max(res.get(I), p2.get(I)));
    }
    return res;
  }

  // min function between two evidence
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >
      Tensor< GUM_SCALAR >::operator&(const Tensor< GUM_SCALAR >& p2) const {
    if (!isEvidence() || !p2.isEvidence())
      GUM_ERROR(InvalidArgument, "The tensors are not evidences.")
    if (this->variable(0) != p2.variable(0))
      GUM_ERROR(InvalidArgument, "The evidence are not on the same variable.")
    Tensor< GUM_SCALAR > res(*this);
    gum::Instantiation      I(res);
    for (I.setFirst(); !I.end(); ++I) {
      res.set(I, std::min(res.get(I), p2.get(I)));
    }
    return res;
  }

  // complement function between two evidence
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::operator~() const {
    if (!isEvidence()) GUM_ERROR(InvalidArgument, "The tensor is not an evidence.")

    Tensor< GUM_SCALAR > res(*this);
    gum::Instantiation      I(res);
    for (I.setFirst(); !I.end(); ++I) {
      res.set(I, 1 - res.get(I));
    }
    return res;
  }

  /// the function to be used to subtract two Tensors
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >
      Tensor< GUM_SCALAR >::operator-(const Tensor< GUM_SCALAR >& p2) const {
    if (p2.empty()) return Tensor< GUM_SCALAR >(*this).translate(-p2.empty_value_);
    if (this->empty()) {
      auto p = Tensor< GUM_SCALAR >(p2);
      p.apply([this](GUM_SCALAR x) { return this->empty_value_ - x; });
      return p;
    }
    return Tensor< GUM_SCALAR >(*this->content() - *p2.content());
  }

  /// the function to be used to substract a GUM_SCALAR from a Tensor
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::operator-(const GUM_SCALAR& v) const {
    return Tensor< GUM_SCALAR >(*this).translate(-v);
  }

  /// the function to be used to multiply two Tensors
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >
      Tensor< GUM_SCALAR >::operator*(const Tensor< GUM_SCALAR >& p2) const {
    if (p2.empty()) return Tensor< GUM_SCALAR >(*this).scale(p2.empty_value_);
    if (this->empty()) return Tensor< GUM_SCALAR >(p2).scale(this->empty_value_);

    return Tensor< GUM_SCALAR >(*this->content() * *p2.content());
  }

  /// the function to be used to multiply a Tensor and a scalar
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::operator*(const GUM_SCALAR& v) const {
    return Tensor< GUM_SCALAR >(*this).scale(v);
  }

  /// the function to be used to divide two Tensors
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >
      Tensor< GUM_SCALAR >::operator/(const Tensor< GUM_SCALAR >& p2) const {
    if (p2.empty()) return Tensor< GUM_SCALAR >(*this).scale(1 / p2.empty_value_);
    if (this->empty()) {
      auto p = Tensor< GUM_SCALAR >(p2);
      p.apply([this](GUM_SCALAR x) { return this->empty_value_ / x; });
      return p;
    }
    return Tensor< GUM_SCALAR >(*this->content() / *p2.content());
  }

  /// the function to be used to divide a Tensor by a scalar
  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::operator/(const GUM_SCALAR& v) const {
    return Tensor< GUM_SCALAR >(*this).scale(1 / v);
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator+=(const Tensor< GUM_SCALAR >& r) {
    *this = *this + r;
    return *this;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator+=(const GUM_SCALAR& v) {
    this->translate(v);
    return *this;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator*=(const Tensor< GUM_SCALAR >& r) {
    *this = *this * r;
    return *this;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator*=(const GUM_SCALAR& v) {
    this->scale(v);
    return *this;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator-=(const Tensor< GUM_SCALAR >& r) {
    *this = *this - r;
    return *this;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator-=(const GUM_SCALAR& v) {
    this->translate(-v);
    return *this;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator/=(const Tensor< GUM_SCALAR >& r) {
    *this = *this / r;
    return *this;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >& Tensor< GUM_SCALAR >::operator/=(const GUM_SCALAR& v) {
    this->scale(1 / v);
    return *this;
  }

  template < typename GUM_SCALAR >
  bool Tensor< GUM_SCALAR >::operator==(const Tensor< GUM_SCALAR >& r) const {
    if (this->empty()) {
      if (r.empty()) return this->empty_value_ == r.empty_value_;
      else return false;
    } else {
      if (r.empty()) return false;
      else return (*this->content_) == (*r.content_);
    }
  }

  template < typename GUM_SCALAR >
  std::string Tensor< GUM_SCALAR >::toString() const {
    auto              table = this->content();
    std::stringstream ss;

    if (table->nbrDim() == 0) {
      Instantiation I(this);
      ss << "[" << this->get(I) << "]";
      return ss.str();
    }
    const Size colwidth    = 6;
    const Size numberwidth = 9;
    const Size nbrLigMax   = 6;

    ss << std::left << std::fixed << std::endl;
    ss.precision(numberwidth - 5);

    const auto& var = table->variable(0);

    const Size        nbparents = table->nbrDim() - 1;
    const Size        nbcol     = var.domainSize();
    const std::string maskparent(colwidth, '-');
    const std::string masknumber(numberwidth, '-');

    if (nbparents > 0)
      ss << std::setw(nbparents * (colwidth + 1) - 1) << " "
         << "||";
    ss << "  " << std::setw(nbcol * (numberwidth + 1) - 3)
       << var.name().substr(0, nbcol * (numberwidth + 1) - 3) << "|";
    ss << std::endl;

    if (nbparents > 0) {
      for (Idx i = 1; i <= nbparents; i++)
        ss << std::setw(colwidth) << table->variable(i).name().substr(0, colwidth) << "|";
      ss << "|";
    }
    for (Idx i = 0; i < nbcol; i++)
      ss << std::setw(numberwidth) << var.label(i).substr(0, numberwidth) << "|";
    ss << std::endl;


    if (nbparents > 0) {
      for (Idx i = 1; i <= nbparents; i++)
        ss << maskparent << "|";
      ss << "|";
    }
    for (Idx i = 0; i < nbcol; i++)
      ss << masknumber << "|";
    ss << std::endl;
    Instantiation I(*table);

    const auto drawligne = [&]() {
      if (nbparents > 0) {
        for (Idx i = 1; i <= nbparents; i++)
          ss << std::setw(colwidth) << table->variable(i).label(I.val(i)).substr(0, colwidth)
             << "|";
        ss << "|";
      }
      for (I.setFirstVar(var); !I.end(); I.incVar(var))
        ss << " " << std::setw(numberwidth - 1) << table->get(I) << "|";
      I.setFirstVar(var);
      ss << std::endl;
    };

    if (const Size nbrLig = table->domainSize() / var.domainSize(); nbrLig < nbrLigMax * 2 + 1) {
      for (I.setFirst(); !I.end(); I.incNotVar(var))
        drawligne();
    } else {
      Size cpt = 0;
      for (I.setFirst(); !I.end(); I.incNotVar(var)) {
        cpt++;
        if (cpt > nbrLigMax) break;
        drawligne();
      }
      ss << "[..." << nbrLig - nbrLigMax * 2 << " more line(s) ...]" << std::endl;
      I.setLast();
      for (Idx revi = 1; revi < nbrLigMax; revi++)
        I.decNotVar(var);
      for (I.setFirstVar(var); !I.end(); I.incNotVar(var)) {
        drawligne();
      }
    }

    return ss.str();
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::evEq(const DiscreteVariable& v, double val) {
    const auto i = v.closestIndex(val);

    gum::Tensor< GUM_SCALAR > p;
    p.add(v);
    p.fillWith(0);
    Instantiation I(p);
    I.chgVal(0, i);
    p.set(I, 1);
    return p;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR >
      Tensor< GUM_SCALAR >::evIn(const DiscreteVariable& v, double val1, double val2) {
    if (val2 < val1) {
      GUM_ERROR(InvalidArgument,
                "val2 (" << val2 << ") must be greater than val1 (" << val1 << ").")
    }
    const auto i1 = v.closestIndex(val1);
    const auto i2 = v.closestIndex(val2);

    gum::Tensor< GUM_SCALAR > p;
    p.add(v);
    p.fillWith(0);
    Instantiation I(p);
    for (Idx i = i1; i <= i2; i++) {
      I.chgVal(0, i);
      p.set(I, 1);
    }
    return p;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::evGt(const DiscreteVariable& v, double val) {
    const auto i = v.closestIndex(val);
    if (i == v.domainSize() - 1) { return evEq(v, val); }

    gum::Tensor< GUM_SCALAR > p;
    p.add(v);
    p.fillWith(0);
    Instantiation I(p);
    for (Idx i2 = i + 1; i2 < v.domainSize(); i2++) {
      I.chgVal(0, i2);
      p.set(I, 1);
    }
    return p;
  }

  template < typename GUM_SCALAR >
  Tensor< GUM_SCALAR > Tensor< GUM_SCALAR >::evLt(const DiscreteVariable& v, double val) {
    const auto i = v.closestIndex(val);
    if (i == 0) { return evEq(v, val); }

    gum::Tensor< GUM_SCALAR > p;
    p.add(v);
    p.fillWith(0);
    Instantiation I(p);
    for (Idx i2 = 0; i2 < i; i2++) {
      I.chgVal(0, i2);
      p.set(I, 1);
    }
    return p;
  }

  template < typename GUM_SCALAR >
  Size Tensor< GUM_SCALAR >::memoryFootprint() const {
    return this->content()->realSize() * sizeof(GUM_SCALAR);
  }

} /* namespace gum */
