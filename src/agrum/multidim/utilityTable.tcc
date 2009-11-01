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

  template <typename T_DATA> INLINE
  UtilityTable<T_DATA>::UtilityTable() :
    MultiDimDecorator<T_DATA>( new MultiDimArray<T_DATA> ) {
    GUM_CONSTRUCTOR( UtilityTable );
  }

  template <typename T_DATA> INLINE
  UtilityTable<T_DATA>::UtilityTable( MultiDimImplementation<T_DATA>* aContent ) :
    MultiDimDecorator<T_DATA>( aContent ) {
    GUM_CONSTRUCTOR( UtilityTable );
  }
  
  template <typename T_DATA> INLINE
  UtilityTable<T_DATA>::~UtilityTable() {
    GUM_DESTRUCTOR( UtilityTable );
  }
  
  template <typename T_DATA> INLINE
  UtilityTable<T_DATA>::UtilityTable(const UtilityTable<T_DATA>& toCopy) :
    MultiDimDecorator<T_DATA>(static_cast<MultiDimImplementation<T_DATA>*>(toCopy.getContent()->newFactory()))
  {
    const Sequence<const DiscreteVariable*>& varSeq = 
      toCopy.variablesSequence();
    for(Sequence<const DiscreteVariable*>::iterator iter =
	  varSeq.begin();
	iter != varSeq.end(); ++iter) {
      this->add(**iter);
    }
    Instantiation i1(toCopy);
    Instantiation i2(*this);
    for(i1.setFirst(), i2.setFirstIn(i1); 
	! i1.end(); 
	++i1, i2.incIn(i1)) {
      this->set(i2, toCopy[i1]);
    }
    GUM_CONS_CPY( UtilityTable ); 
  }

  template <typename T_DATA> INLINE
  UtilityTable<T_DATA>& UtilityTable<T_DATA>::operator=(const UtilityTable<T_DATA>& toCopy) {
    GUM_ERROR( OperationNotAllowed,
	       "No copy for UtilityTable : how to choose the implementation ?" );
    return *this;
  }

  template <typename T_DATA> INLINE
  UtilityTable<T_DATA>* UtilityTable<T_DATA>::newFactory() const {
    return new UtilityTable<T_DATA>(static_cast<MultiDimImplementation<T_DATA>*>(this->getContent()->newFactory()));
  }

  template <typename T_DATA> INLINE
  void UtilityTable<T_DATA>::sum(const UtilityTable<T_DATA>& p1,
				 const UtilityTable<T_DATA>& p2) {
    this->beginMultipleChanges();
    // remove vars in this : WARNING -- THIS IS A COPY OF SEQ !!!!
    const Sequence<const DiscreteVariable *> seq0=this->variablesSequence() ;
    
    for ( Sequence<const DiscreteVariable *>::iterator iter = seq0.begin();
          iter!=seq0.end();++iter ) {
      this->erase( **iter );
    }
    
    // adding vars in p1
    const Sequence<const DiscreteVariable *>& seq1=p1.variablesSequence() ;
    
    for ( Sequence<const DiscreteVariable *>::iterator iter = seq1.begin();
          iter!=seq1.end();++iter ) {
      this->add( **iter );
    }

    // adding vars in p2 not already there
    const Sequence<const DiscreteVariable *>& seq2=p2.variablesSequence() ;

    for ( Sequence<const DiscreteVariable *>::iterator iter = seq2.begin();
          iter!=seq2.end();++iter ) {
      if ( ! this->contains( **iter ) ) {
        this->add( **iter );
      }
    }

    this->endMultipleChanges();

    Instantiation i( this );
    // it looks like we don't need much more optimization (all the sums & prods
    // have to be made once at least) ...
    // remember that p1[i] means p1[just the part of i that concerns p1]
    
    for ( i.setFirst();! i.end(); ++i ) this->set( i ,p1[i]+p2[i]);
    
  }
  
  template <typename T_DATA> 
  void UtilityTable<T_DATA>::sumBy(const UtilityTable<T_DATA>& toAdd) {
    UtilityTable<T_DATA> tab(static_cast<MultiDimImplementation<T_DATA>*>(this->_content->newFactory()));
    tab.sum(*this, toAdd);
    MultiDimImplementation<T_DATA>* swap = this->_content;
    this->_content = tab._content;
    tab._content = swap;
  }

  template <typename T_DATA> 
  void UtilityTable<T_DATA>::sumBy(const List<UtilityTable<T_DATA>*>& utilitiesList) {
    if(! utilitiesList.empty()) {
      Instantiation globalInst(*this);
      Instantiation partialInst;
      for(typename List<UtilityTable<T_DATA>*>::const_iterator iter = utilitiesList.begin();
	  iter != utilitiesList.end(); ++iter) {
	const Sequence<const DiscreteVariable*>& varSeq = (*iter)->variablesSequence();
	for(typename Sequence<const DiscreteVariable*>::iterator varIter = varSeq.begin();
	    varIter != varSeq.end(); ++varIter) {
	  try {
	    partialInst.add(**varIter);
	  } catch(DuplicateElement&) {
	  }
	}
      }
      if(partialInst.nbrDim()  != globalInst.nbrDim()) {

	// Now partialInst contains all values of subutilities
	for(globalInst.setFirstIn(partialInst),
	      partialInst.setFirst();
	    ! partialInst.end();
	    globalInst.incIn(partialInst),
	      ++partialInst) {

	  T_DATA sumData = (T_DATA) 0;
	  for(typename List<UtilityTable<T_DATA>*>::const_iterator iter = utilitiesList.begin();
	      iter != utilitiesList.end(); ++iter) {
	    sumData += (**iter)[partialInst];
	  }
	  for(globalInst.setFirstOut(partialInst);
	      ! globalInst.end();
	      globalInst.incOut(partialInst)) {
	    this->set(globalInst, this->get(globalInst) + sumData);
	  }
	  globalInst.unsetOverflow();
	}
      } else {

	for(globalInst.setFirst(); ! globalInst.end(); ++globalInst) {
	  T_DATA sumData = (T_DATA) 0;
	  for(typename List<UtilityTable<T_DATA>*>::const_iterator iter = utilitiesList.begin();
	      iter != utilitiesList.end(); ++iter) {
	    sumData += (*iter)->get(globalInst);
	  }
	  this->set(globalInst, this->get(globalInst) + sumData);
	}
      }
    }
//     for(typename List<UtilityTable<T_DATA>*>::const_iterator iter = utilitiesList.begin();
// 	iter != utilitiesList.end(); ++iter) {
//       this->sumBy(**iter);
//     }
  }

  template <typename T_DATA>
  UtilityTable<T_DATA> UtilityTable<T_DATA>::reduceBy(const List<const DiscreteVariable*>& varList) const {
    UtilityTable<T_DATA> result(new MultiDimArray<T_DATA>);
    for(List<const DiscreteVariable*>::const_iterator iter = varList.begin();
	iter != varList.end(); ++iter) {
      const DiscreteVariable& var = **iter;
      if(this->contains(var)) {
	result.add(var);
      } else {
	GUM_ERROR(NotFound, "a variable isn't contained by the utility");
      }
    }
    Instantiation resultI(result);
    Instantiation inst(*this);
    for(resultI.setFirst(), inst.setFirstIn(resultI);
	! resultI.end(); 
	++resultI, inst.incIn(resultI)) {
      inst.setFirstOut(resultI);
      T_DATA currentMax = this->get(inst);
      for(inst.incOut(resultI);
	  ! inst.end();
	  inst.incOut(resultI)) {
	T_DATA val = this->get(inst);
	if(val > currentMax) {
	  currentMax = val;
	}
      }
      result.set(resultI, currentMax);
      inst.unsetOverflow();
    }
    return result;
  }

} /* namespace gum */


#endif  // DOXYGEN_SHOULD_SKIP_THIS
