
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


/**
 * @file
 * @brief Implementation of the Potential class.
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

#include <agrum/agrum.h>
#include <agrum/core/math/math.h>
#include <agrum/multidim/potential.h>

namespace gum {

  // Default constructor: creates an empty null dimensional matrix
  // choose a MultiDimArray<> as decorated implementation
  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR >::Potential() :
      MultiDimDecorator< GUM_SCALAR >(new MultiDimArray< GUM_SCALAR >(),
                                      GUM_SCALAR(1)) {
    GUM_CONSTRUCTOR(Potential);
  }

  // constructor using aContent as content
  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR >::Potential(
     MultiDimImplementation< GUM_SCALAR >* aContent) :
      MultiDimDecorator< GUM_SCALAR >(aContent, GUM_SCALAR(1)) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Potential);
  }
  // copy constructor
  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR >::Potential(const Potential< GUM_SCALAR >& src) :
      Potential< GUM_SCALAR >(static_cast< MultiDimImplementation< GUM_SCALAR >* >(
                                 src.content()->newFactory()),
                              *(src.content())) {
    this->_empty_value = src._empty_value;
    // todo how to optimize copy of content ?
    // GUM_CONS_CPY not here because in called Potential
    // GUM_CONS_CPY( Potential );
  }

  /// move constructor
  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR >::Potential(Potential< GUM_SCALAR >&& from) :
      MultiDimDecorator< GUM_SCALAR >(
         std::forward< MultiDimDecorator< GUM_SCALAR > >(from)) {
    GUM_CONS_MOV(Potential);
  }

  // complex copy constructor : we choose the implementation
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >::Potential(
     MultiDimImplementation< GUM_SCALAR >*  aContent,
     const MultiDimContainer< GUM_SCALAR >& src) :
      MultiDimDecorator< GUM_SCALAR >(aContent) {
    // for debugging purposes
    GUM_CONSTRUCTOR(Potential);

    if (!src.empty()) {
      this->beginMultipleChanges();

      for (Idx i = 0; i < src.variablesSequence().size(); i++) {
        this->add(*(src.variablesSequence()[i]));
      }

      this->endMultipleChanges();
      this->content()->copyFrom(*src.content());
    }
  }

  // operator = copy
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >&
     Potential< GUM_SCALAR >::operator=(const Potential< GUM_SCALAR >& src) {
    GUM_OP_CPY(Potential);
    if (&src == this) return *this;
    MultiDimDecorator< GUM_SCALAR >::operator=(src);
    return *this;
  }

  // operator = move
  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >&
     Potential< GUM_SCALAR >::operator=(Potential< GUM_SCALAR >&& src) {
    GUM_OP_MOV(Potential);
    if (&src == this) return *this;
    MultiDimDecorator< GUM_SCALAR >::operator=(
       std::forward< MultiDimDecorator< GUM_SCALAR > >(src));
    return *this;
  }

  // destructor

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >::~Potential() {
    // for debugging purposes
    GUM_DESTRUCTOR(Potential);
  }

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR >* Potential< GUM_SCALAR >::newFactory() const {
    return new Potential< GUM_SCALAR >(
       static_cast< MultiDimImplementation< GUM_SCALAR >* >(
          this->content()->newFactory()));
  }

  // sum of all elements in this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Potential< GUM_SCALAR >::sum() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return this->_empty_value;
    }
    return gum::projectSum(*this->content());
  }
  // product of all elements in this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Potential< GUM_SCALAR >::product() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return this->_empty_value;
    }
    return gum::projectProduct(*this->content());
  }
  // max of all elements in this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Potential< GUM_SCALAR >::max() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return this->_empty_value;
    }
    return gum::projectMax(*this->content());
  }
  // min of all elements in this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Potential< GUM_SCALAR >::min() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return this->_empty_value;
    }
    return gum::projectMin(*this->content());
  }

  // max of all non one elements in this
  template < typename GUM_SCALAR >
  GUM_SCALAR Potential< GUM_SCALAR >::maxNonOne() const {
    GUM_SCALAR res;

    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      res = this->_empty_value;
    } else {
      res = this->reduce(
         [](GUM_SCALAR z, GUM_SCALAR p) {
           return (p == static_cast< GUM_SCALAR >(1))
                     ? z
                     : (z == static_cast< GUM_SCALAR >(1)) ? p : (p > z ? p : z);
         },
         static_cast< GUM_SCALAR >(1));
    }

    if (res == static_cast< GUM_SCALAR >(1))
      GUM_ERROR(NotFound, "No other value than 1");

    return res;
  }

  // min of all non zero elements in this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Potential< GUM_SCALAR >::minNonZero() const {
    GUM_SCALAR res;

    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      res = this->_empty_value;
    } else {
      res = this->reduce(
         [](GUM_SCALAR z, GUM_SCALAR p) {
           return (p == static_cast< GUM_SCALAR >(0))
                     ? z
                     : (z == static_cast< GUM_SCALAR >(0)) ? p : (p < z ? p : z);
         },
         static_cast< GUM_SCALAR >(0));
    }

    if (res == static_cast< GUM_SCALAR >(0))
      GUM_ERROR(NotFound, "No other value than 1");

    return res;
  }

  // entropy of this
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR Potential< GUM_SCALAR >::entropy() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return static_cast< GUM_SCALAR >(0);
    }

    return this->reduce(
       [](GUM_SCALAR z, GUM_SCALAR p) {
         return (p == 0.0) ? z : (z - p * log2(p));
       },
       0.0);
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               Potential< GUM_SCALAR >::fillWith(const std::vector< GUM_SCALAR >& v) const {
    this->populate(v);
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               Potential< GUM_SCALAR >::fillWith(const GUM_SCALAR& v) const {
    this->fill(v);
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               Potential< GUM_SCALAR >::fillWith(const Potential< GUM_SCALAR >& src) const {
    if (src.domainSize() != this->domainSize()) {
      GUM_ERROR(InvalidArgument, "Potential to copy has not the same dimension.");
    }
    gum::Set< std::string > son;   // set of names
    for (const auto& v : src.variablesSequence()) {
      son.insert(v->name());
    }
    for (const auto& v : this->variablesSequence()) {
      if (!son.contains(v->name())) {
        GUM_ERROR(InvalidArgument,
                  "Variable <" << v->name() << "> not present in src.");
      }
      // we check size, labels and order of labels in the same time
      if (v->toString() != src.variable(v->name()).toString()) {
        GUM_ERROR(InvalidArgument,
                  "Variables <" << v->name() << "> are not identical.");
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
  INLINE const Potential< GUM_SCALAR >& Potential< GUM_SCALAR >::fillWith(
     const Potential< GUM_SCALAR >&    src,
     const std::vector< std::string >& mapSrc) const {
    if (src.nbrDim() != this->nbrDim()) {
      GUM_ERROR(InvalidArgument, "Potential to copy has not the same dimension.");
    }
    if (src.nbrDim() != mapSrc.size()) {
      GUM_ERROR(InvalidArgument,
                "Potential and vector have not the same dimension.");
    }
    Instantiation Isrc;
    for (Idx i = 0; i < src.nbrDim(); i++) {
      if (src.variable(mapSrc[i]).domainSize() != this->variable(i).domainSize()) {
        GUM_ERROR(InvalidArgument,
                  "Variables " << mapSrc[i] << " (in the argument) and "
                               << this->variable(i).name()
                               << " have not the same dimension.");
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
  INLINE const Potential< GUM_SCALAR >& Potential< GUM_SCALAR >::sq() const {
    this->apply([](GUM_SCALAR x) { return x * x; });
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR
         Potential< GUM_SCALAR >::KL(const Potential< GUM_SCALAR >& p) const {
    if (this->nbrDim() != p.nbrDim())
      GUM_ERROR(
         InvalidArgument,
         "BNdistance between potentials with different numbers of dimensions");
    for (const auto var : p.variablesSequence()) {
      if (!this->contains(*var))
        GUM_ERROR(InvalidArgument,
                  "A variable in the argument does not belong to the potential.");
    }
    for (const auto var : this->variablesSequence()) {
      if (!p.contains(*var))
        GUM_ERROR(InvalidArgument, "A variable does not belong to the argument.");
    }

    Instantiation inst(*this);
    GUM_SCALAR    res = static_cast< GUM_SCALAR >(0);
    for (inst.setFirst(); !inst.end(); inst.inc()) {
      GUM_SCALAR x = this->get(inst);
      GUM_SCALAR y = p.get(inst);
      if (static_cast< GUM_SCALAR >(0) == x)   // 0*log(0/y)=0
        continue;

      if (static_cast< GUM_SCALAR >(0) == y)
        // we know that x!=0;
        GUM_ERROR(FatalError,
                  "The argument has a 0 at " << inst
                                             << " while the potential has not.")

      res += x * log2(x / y);
    }
    return res;
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >& Potential< GUM_SCALAR >::abs() const {
    this->apply([](GUM_SCALAR x) {
      if (x >= 0)
        return x;
      else
        return -x;
    });
    return *this;
  }

  // normalisation of this
  // do nothing is sum is 0
  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               Potential< GUM_SCALAR >::normalize() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      if (this->_empty_value != static_cast< GUM_SCALAR >(0))
        this->_empty_value = static_cast< GUM_SCALAR >(1.0);
    } else {
      GUM_SCALAR s = sum();

      if (s != (GUM_SCALAR)0) {
        this->apply([s](GUM_SCALAR x) { return x / s; });
      }
    }
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               Potential< GUM_SCALAR >::normalizeAsCPT() const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      if (this->_empty_value != static_cast< GUM_SCALAR >(0)) {
        this->_empty_value = static_cast< GUM_SCALAR >(1.0);
      } else {
        GUM_ERROR(FatalError,
                  "Normalization for a potential that sum to 0 in " << *this);
      }
    } else {
      Instantiation inst(*this);
      const auto&   v = this->variable(0);

      for (inst.setFirst(); !inst.end(); inst.incNotVar(v)) {
        GUM_SCALAR s = (GUM_SCALAR)0.0;
        for (inst.setFirstVar(v); !inst.end(); inst.incVar(v))
          s += this->get(inst);
        if (s == (GUM_SCALAR)0.0) {
          GUM_ERROR(FatalError,
                    "Normalization for a potential that sum to 0 in " << *this);
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
  INLINE const Potential< GUM_SCALAR >&
               Potential< GUM_SCALAR >::scale(GUM_SCALAR v) const {
    this->apply([v](GUM_SCALAR x) { return x * v; });
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               Potential< GUM_SCALAR >::translate(GUM_SCALAR v) const {
    this->apply([v](GUM_SCALAR x) { return x + v; });
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR > Potential< GUM_SCALAR >::margSumOut(
     const Set< const DiscreteVariable* >& del_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return Potential< GUM_SCALAR >().fillWith(this->_empty_value);
    }
    return Potential< GUM_SCALAR >(gum::projectSum(*this->content(), del_vars));
  }

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR > Potential< GUM_SCALAR >::margProdOut(
     const Set< const DiscreteVariable* >& del_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return Potential< GUM_SCALAR >().fillWith(this->_empty_value);
    }
    return Potential< GUM_SCALAR >(
       gum::projectProduct(*this->content(), del_vars));
  }

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR > Potential< GUM_SCALAR >::margMinOut(
     const Set< const DiscreteVariable* >& del_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return Potential< GUM_SCALAR >().fillWith(this->_empty_value);
    }
    return Potential< GUM_SCALAR >(gum::projectMin(*this->content(), del_vars));
  }

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR > Potential< GUM_SCALAR >::margMaxOut(
     const Set< const DiscreteVariable* >& del_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return Potential< GUM_SCALAR >().fillWith(this->_empty_value);
    }
    return Potential< GUM_SCALAR >(gum::projectMax(*this->content(), del_vars));
  }
  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR > Potential< GUM_SCALAR >::margSumIn(
     const Set< const DiscreteVariable* >& kept_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return Potential< GUM_SCALAR >().fillWith(this->_empty_value);
    }
    return Potential< GUM_SCALAR >(
       gum::projectSum(*this->content(), _complementVars(kept_vars)));
  }

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR > Potential< GUM_SCALAR >::margProdIn(
     const Set< const DiscreteVariable* >& kept_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return Potential< GUM_SCALAR >().fillWith(this->_empty_value);
    }
    return Potential< GUM_SCALAR >(
       gum::projectProduct(*this->content(), _complementVars(kept_vars)));
  }

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR > Potential< GUM_SCALAR >::margMinIn(
     const Set< const DiscreteVariable* >& kept_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return Potential< GUM_SCALAR >().fillWith(this->_empty_value);
    }
    return Potential< GUM_SCALAR >(
       gum::projectMin(*this->content(), _complementVars(kept_vars)));
  }

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR > Potential< GUM_SCALAR >::margMaxIn(
     const Set< const DiscreteVariable* >& kept_vars) const {
    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return Potential< GUM_SCALAR >().fillWith(this->_empty_value);
    }
    return Potential< GUM_SCALAR >(
       gum::projectMax(*this->content(), _complementVars(kept_vars)));
  }

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR > Potential< GUM_SCALAR >::isNonZeroMap() const {
    auto p = Potential< GUM_SCALAR >(*this);
    p.apply([](GUM_SCALAR x) {
      if (x != static_cast< GUM_SCALAR >(0))
        return static_cast< GUM_SCALAR >(1);
      else
        return static_cast< GUM_SCALAR >(0);
    });
    return p;
  }

  template < typename GUM_SCALAR >
  Set< const DiscreteVariable* > Potential< GUM_SCALAR >::_complementVars(
     const Set< const DiscreteVariable* >& vars) const {
    Set< const DiscreteVariable* > cplt;

    for (const auto x : this->variablesSequence())
      if (!vars.contains(x)) cplt.insert(x);

    return cplt;
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR > Potential< GUM_SCALAR >::reorganize(
     const std::vector< const DiscreteVariable* >& vars) const {
    if (vars.size() != this->nbrDim())
      GUM_ERROR(InvalidArgument,
                "The vector contains " << vars.size() << " variables instead of "
                                       << this->nbrDim() << ".");
    for (const auto var : vars) {
      if (!this->contains(*var))
        GUM_ERROR(InvalidArgument,
                  "A variable in the vector does not belong to the potential.");
    }

    Potential< GUM_SCALAR > p;
    p.beginMultipleChanges();
    for (const auto var : vars)
      p.add(*var);
    p.endMultipleChanges();
    p.copyFrom(*this, nullptr);   // copy *this in p using the same order

    return p;
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     Potential< GUM_SCALAR >::putFirst(const DiscreteVariable* var) const {
    if (!this->contains(*var)) {
      GUM_ERROR(InvalidArgument,
                "The variable to put first does not belong to the potential");
    }
    if (&(this->variable(0)) == var) return Potential< GUM_SCALAR >(*this);

    std::vector< const DiscreteVariable* > vars;
    vars.push_back(var);
    for (Idx i = 0; i < this->nbrDim(); i++)
      if (&(this->variable(i)) != var) vars.push_back(&(this->variable(i)));

    return this->reorganize(vars);
  }

  template < typename GUM_SCALAR >
  Potential< GUM_SCALAR >
     Potential< GUM_SCALAR >::extract(const Instantiation& inst) const {
    Potential< GUM_SCALAR > p;
    p.extractFrom(*this, inst);

    return p;
  }

  template < typename GUM_SCALAR >
  Idx Potential< GUM_SCALAR >::draw() const {
    if (this->nbrDim() != 1) {
      GUM_ERROR(FatalError, "To draw from a potential, the dimension must be 1")
    }

    GUM_SCALAR    r = static_cast< GUM_SCALAR >(randomProba());
    Instantiation Ip(*this);
    for (Ip.setFirst(); !Ip.end(); Ip.inc()) {
      r -= this->get(Ip);
      if (r <= 0) return Ip.val(0);
    }
    return this->variable(0).domainSize() - 1;
  }

  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream&                  out,
                           const Potential< GUM_SCALAR >& array) {
    out << array.toString();
    return out;
  }

  // argmax of all elements in this
  template < typename GUM_SCALAR >
  Set< Instantiation > Potential< GUM_SCALAR >::findAll(GUM_SCALAR v) const {
    Instantiation        I(*this);
    Set< Instantiation > res;

    if (static_cast< MultiDimContainer< GUM_SCALAR >* >(this->_content)->empty()) {
      return res;
    }
    for (I.setFirst(); !I.end(); ++I) {
      if (this->get(I) == v) res.insert(I);
    }
    return res;
  }
  // argmax of all elements in this
  template < typename GUM_SCALAR >
  INLINE Set< Instantiation > Potential< GUM_SCALAR >::argmax() const {
    return findAll(max());
  }
  // argmin of all elements in this
  template < typename GUM_SCALAR >
  INLINE Set< Instantiation > Potential< GUM_SCALAR >::argmin() const {
    return findAll(min());
  }

  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >& Potential< GUM_SCALAR >::random() const {
    std::vector< GUM_SCALAR > v;

    GUM_SCALAR sum;
    v.reserve(this->domainSize());
    do {
      sum = 0.0;
      for (Size i = 0; i < this->domainSize(); ++i) {
        auto r = (GUM_SCALAR)randomProba();
        v.push_back(r);
        sum += r;
      }
    } while (sum == 0.0);

    this->fillWith(v);
    return *this;
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               Potential< GUM_SCALAR >::randomDistribution() const {
    return this->random().normalize();
  }

  template < typename GUM_SCALAR >
  INLINE const Potential< GUM_SCALAR >&
               Potential< GUM_SCALAR >::randomCPT() const {
    return this->random().normalizeAsCPT();
  }

  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     Potential< GUM_SCALAR >::noising(GUM_SCALAR alpha) const {
    if ((alpha < GUM_SCALAR(0.0)) || (alpha > GUM_SCALAR(1.0))) {
      GUM_ERROR(InvalidArgument, "alpha must be in [0,1]");
    }
    Potential< GUM_SCALAR > noise(*this);
    return fillWith(scale(1 - alpha) + noise.randomCPT().scale(alpha))
       .normalizeAsCPT();
  }

} /* namespace gum */
