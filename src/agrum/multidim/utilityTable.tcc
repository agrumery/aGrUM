/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR>::UtilityTable()
      : MultiDimDecorator<GUM_SCALAR>(new MultiDimArray<GUM_SCALAR>) {
    GUM_CONSTRUCTOR(UtilityTable);
  }

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR>::UtilityTable(
      MultiDimImplementation<GUM_SCALAR> *aContent)
      : MultiDimDecorator<GUM_SCALAR>(aContent) {
    GUM_CONSTRUCTOR(UtilityTable);
  }

  template <typename GUM_SCALAR> INLINE UtilityTable<GUM_SCALAR>::~UtilityTable() {
    GUM_DESTRUCTOR(UtilityTable);
  }

  template <typename GUM_SCALAR>
  INLINE
  UtilityTable<GUM_SCALAR>::UtilityTable(const UtilityTable<GUM_SCALAR> &toCopy)
      : MultiDimDecorator<GUM_SCALAR>(
            static_cast<MultiDimImplementation<GUM_SCALAR> *>(
                toCopy.content()->newFactory())) {
    const Sequence<const DiscreteVariable *> &varSeq = toCopy.variablesSequence();

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = varSeq.beginSafe();
         iter != varSeq.endSafe(); ++iter) {
      this->add(**iter);
    }

    Instantiation i1(toCopy);
    Instantiation i2(*this);

    for (i1.setFirst(), i2.setFirstIn(i1); !i1.end(); ++i1, i2.incIn(i1)) {
      this->set(i2, toCopy[i1]);
    }

    GUM_CONS_CPY(UtilityTable);
  }

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR> &UtilityTable<GUM_SCALAR>::
  operator=(const UtilityTable<GUM_SCALAR> &toCopy) {
    GUM_ERROR(OperationNotAllowed,
              "No copy for UtilityTable : how to choose the implementation ?");
    return *this;
  }

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR> *UtilityTable<GUM_SCALAR>::newFactory() const {
    return new UtilityTable<GUM_SCALAR>(
        static_cast<MultiDimImplementation<GUM_SCALAR> *>(
            this->content()->newFactory()));
  }

  template <typename GUM_SCALAR>
  INLINE void UtilityTable<GUM_SCALAR>::sum(const UtilityTable<GUM_SCALAR> &p1,
                                            const UtilityTable<GUM_SCALAR> &p2) {
    this->beginMultipleChanges();
    // remove vars in this : WARNING -- THIS IS A COPY OF SEQ !!!!
    const Sequence<const DiscreteVariable *> seq0 = this->variablesSequence();

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = seq0.beginSafe();
         iter != seq0.endSafe(); ++iter) {
      this->erase(**iter);
    }

    // adding vars in p1
    const Sequence<const DiscreteVariable *> &seq1 = p1.variablesSequence();

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = seq1.beginSafe();
         iter != seq1.endSafe(); ++iter) {
      this->add(**iter);
    }

    // adding vars in p2 not already there
    const Sequence<const DiscreteVariable *> &seq2 = p2.variablesSequence();

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = seq2.beginSafe();
         iter != seq2.endSafe(); ++iter) {
      if (!this->contains(**iter)) {
        this->add(**iter);
      }
    }

    this->endMultipleChanges();

    Instantiation i(this);
    // it looks like we don't need much more optimization (all the sums & prods
    // have to be made once at least) ...
    // remember that p1[i] means p1[just the part of i that concerns p1]

    for (i.setFirst(); !i.end(); ++i)
      this->set(i, p1[i] + p2[i]);
  }

  template <typename GUM_SCALAR>
  void UtilityTable<GUM_SCALAR>::sumBy(const UtilityTable<GUM_SCALAR> &toAdd) {
    UtilityTable<GUM_SCALAR> tab(static_cast<MultiDimImplementation<GUM_SCALAR> *>(
        this->_content->newFactory()));
    tab.sum(*this, toAdd);
    MultiDimImplementation<GUM_SCALAR> *swap = this->_content;
    this->_content = tab._content;
    tab._content = swap;
  }

  template <typename GUM_SCALAR>
  void UtilityTable<GUM_SCALAR>::sumBy(
      const List<UtilityTable<GUM_SCALAR> *> &utilitiesList) {
    if (!utilitiesList.empty()) {
      Instantiation globalInst(*this);
      Instantiation partialInst;

      for (typename List<UtilityTable<GUM_SCALAR> *>::const_iterator iter =
               utilitiesList.cbegin();
           iter != utilitiesList.cend(); ++iter) {
        const Sequence<const DiscreteVariable *> &varSeq =
            (*iter)->variablesSequence();

        for (typename Sequence<const DiscreteVariable *>::iterator_safe varIter =
                 varSeq.beginSafe();
             varIter != varSeq.endSafe(); ++varIter) {
          try {
            partialInst.add(**varIter);
          } catch (DuplicateElement &) {
          }
        }
      }

      if (partialInst.nbrDim() != globalInst.nbrDim()) {

        // Now partialInst contains all values of subutilities
        for (globalInst.setFirstIn(partialInst), partialInst.setFirst();
             !partialInst.end(); globalInst.incIn(partialInst), ++partialInst) {

          GUM_SCALAR sumData = (GUM_SCALAR)0;

          for (typename List<UtilityTable<GUM_SCALAR> *>::const_iterator iter =
                   utilitiesList.cbegin();
               iter != utilitiesList.cend(); ++iter) {
            sumData += (**iter)[partialInst];
          }

          for (globalInst.setFirstOut(partialInst); !globalInst.end();
               globalInst.incOut(partialInst)) {
            this->set(globalInst, this->get(globalInst) + sumData);
          }

          globalInst.unsetOverflow();
        }
      } else {

        for (globalInst.setFirst(); !globalInst.end(); ++globalInst) {
          GUM_SCALAR sumData = (GUM_SCALAR)0;

          for (typename List<UtilityTable<GUM_SCALAR> *>::const_iterator iter =
                   utilitiesList.cbegin();
               iter != utilitiesList.cend(); ++iter) {
            sumData += (*iter)->get(globalInst);
          }

          this->set(globalInst, this->get(globalInst) + sumData);
        }
      }
    }

    //     for(typename List<UtilityTable<GUM_SCALAR>*>::const_iterator iter =
    //     utilitiesList.begin();
    //  iter != utilitiesList.end(); ++iter) {
    //       this->sumBy(**iter);
    //     }
  }

  template <typename GUM_SCALAR>
  UtilityTable<GUM_SCALAR> UtilityTable<GUM_SCALAR>::reduceBy(
      const List<const DiscreteVariable *> &varList) const {
    UtilityTable<GUM_SCALAR> result(new MultiDimArray<GUM_SCALAR>);

    for (List<const DiscreteVariable *>::const_iterator_safe iter =
             varList.cbeginSafe();
         iter != varList.cendSafe(); ++iter) {
      const DiscreteVariable &var = **iter;

      if (this->contains(var)) {
        result.add(var);
      } else {
        GUM_ERROR(NotFound, "a variable isn't contained by the utility");
      }
    }

    Instantiation resultI(result);
    Instantiation inst(*this);

    for (resultI.setFirst(), inst.setFirstIn(resultI); !resultI.end();
         ++resultI, inst.incIn(resultI)) {
      inst.setFirstOut(resultI);
      GUM_SCALAR currentMax = this->get(inst);

      for (inst.incOut(resultI); !inst.end(); inst.incOut(resultI)) {
        GUM_SCALAR val = this->get(inst);

        if (val > currentMax) {
          currentMax = val;
        }
      }

      result.set(resultI, currentMax);
      inst.unsetOverflow();
    }

    return result;
  }

  template <typename GUM_SCALAR>
  INLINE void UtilityTable<GUM_SCALAR>::_swap(const DiscreteVariable *x,
                                              const DiscreteVariable *y) {
    MultiDimDecorator<GUM_SCALAR>::content()->swap(*x, *y);
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
