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


namespace gum {


  // ============================================================================
  /// Default constructor: creates an empty null dimensional matrix
  /**
   * choose a MultiDimArray<> as decorated implementation  */
  // ============================================================================
  template<typename T_DATA>
  Potential<T_DATA>::Potential( ):
    MultiDimDecorator<T_DATA>( new MultiDimArray<T_DATA>() ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Potential );
  }

  // ==============================================================================
  /// Default constructor: creates an empty null dimensional matrix
  // ==============================================================================
  template<typename T_DATA>
  Potential<T_DATA>::Potential( MultiDimImplementation<T_DATA> *aContent ):
    MultiDimDecorator<T_DATA>( aContent ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Potential );
  }

  // ==============================================================================
  /// copy constructor :
  /// @warning this copy constructor should reference the same content !!!
  /// TOO DANGEROUS !!!
  // ==============================================================================
  template<typename T_DATA>
  Potential<T_DATA>::Potential( const Potential<T_DATA>& from ) :
    MultiDimDecorator<T_DATA>( from )  {
    // for debugging purposes
    GUM_CONS_CPY( Potential );
    GUM_ERROR( OperationNotAllowed,
               "No copy for Potential : how to choose the implementation ?" );
  }

  /// complex copy constructor : we choose the implementation
  template<typename T_DATA>
  Potential<T_DATA>::Potential( MultiDimImplementation<T_DATA> *aContent,
                                const MultiDimContainer<T_DATA>& from ):
    MultiDimDecorator<T_DATA>( aContent ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Potential );

    if ( ! from.empty() ) {
      this->beginMultipleChanges();

      for ( Idx i = 0; i < from.variablesSequence().size(); i++ ) {
        add( *( from.variablesSequence()[i] ) );
      }

      this->endMultipleChanges();

      MultiDimDecorator<T_DATA>::getContent()->copyFrom
        ( dynamic_cast<const MultiDimContainer<T_DATA>&>( from.getMasterRef() ) );
    }
  }

  // operator =
  template<typename T_DATA>
  Potential<T_DATA>& Potential<T_DATA>::operator=( const Potential<T_DATA>& from ) {
    MultiDimDecorator<T_DATA>::getContent()->copy
      ( dynamic_cast<const MultiDimContainer<T_DATA>&>( from.getMasterRef() ) );
    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  template<typename T_DATA>
  Potential<T_DATA>::~Potential() {
    // for debugging purposes
    GUM_DESTRUCTOR( Potential );
  }

  // ==============================================================================
  template<typename T_DATA>
  Potential<T_DATA>& Potential<T_DATA>::marginalize( const Potential& p ) const {
    const Sequence<const DiscreteVariable *>& seq=this->variablesSequence() ;

    if ( p.empty() ) GUM_ERROR( OperationNotAllowed,"Impossible to marginalize" );

    for ( Sequence<const DiscreteVariable *>::iterator iter = seq.begin();
          iter!=seq.end();++iter ) {
      if ( ! p.contains( **iter ) )
        GUM_ERROR( OperationNotAllowed,"Impossible to marginalize" );
    }

    _marginalize( p );

    // a const method should return a const ref BUT WE NEED t return a non const ref
    return const_cast<Potential<T_DATA>&>( *this );
  }

  // ==============================================================================
  template<typename T_DATA>
  void Potential<T_DATA>::_marginalize( const Potential& p )  const {
    Instantiation it_p( p );
    Instantiation it_this( this );
    T_DATA s;

    for ( it_p.setFirst(),it_this.setFirst() ;! it_this.end();
          ++it_this,it_p.incIn( it_this ) ) {
      s=0.0;

      for ( it_p.setFirstOut( it_this );! it_p.end();it_p.incOut( it_this ) ) {
        s+=p[it_p];
      }

      set( it_this ,s);

      it_p.unsetOverflow();
    }
  }

  // ==============================================================================
  template<typename T_DATA>
  INLINE
  Potential<T_DATA>& Potential<T_DATA>::multiplicateBy( const Potential& p1 ) {
    if ( p1.empty() ) {
      GUM_ERROR( OperationNotAllowed,
                 "Multiplicate by empty potential is not allows" );
    }

    if ( this->empty() ) {
      *this=p1;
    } else {
      Potential<T_DATA> tmp;
      tmp._multiplicate( *this,p1 );
      MultiDimImplementation<T_DATA>* p=this->_content;
      this->_content=tmp._content;
      tmp._content=p;
    }

    return ( *this );
  }

  // ==============================================================================
  template<typename T_DATA>
  INLINE
  void Potential<T_DATA>::multiplicate( const Potential& p1,const Potential& p2 ) {
    if ( p1.empty() ) {
      *this=p2;
      return;
    }

    if ( p2.empty() ) {
      *this=p1;
      return;
    }

    _multiplicate( p1,p2 );
  }

  // ==============================================================================
  template<typename T_DATA>
  void Potential<T_DATA>::_multiplicate( const Potential& p1,const Potential& p2 )  {
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

    for ( i.setFirst();! i.end(); ++i ) this->set( i ,p1[i]*p2[i]);
  }

  /// sum of all elements in this
  template<typename T_DATA> INLINE
  const T_DATA Potential<T_DATA>::getSum() const  {
    Instantiation i( this->_content );
    T_DATA s=( T_DATA )0;

    for ( i.setFirst();! i.end(); ++i ) s+=this->get( i );

    return s;
  }

  /// normalisation of this
  /// do nothing is sum is 0
  template<typename T_DATA> INLINE
  Potential<T_DATA>& Potential<T_DATA>::normalize()  const {
    T_DATA s=getSum();

    if ( s!=( T_DATA )0 ) {
      Instantiation i( this->_content );

      for ( i.setFirst();! i.end() ; ++i ) this->set(i,this->get( i )/s);
    }

    // a const method should return a const ref BUT WE NEED t return a non const ref
    return const_cast<Potential<T_DATA>&>( *this );
  }



} /* namespace gum */

