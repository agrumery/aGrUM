/* MAKE OPERATIONS MORE PYTHONIC */
%ignore gum::operator<<;
%ignore* ::operator=;
%ignore* ::operator++;
%ignore* ::operator--;
%ignore* ::operator[];

%ignore* ::operator<<;
%ignore* ::operator>>;

%rename ( append ) gum::Sequence::insert ( const KEY& k );
%rename ( index ) gum::Sequence::pos ( const KEY& key ) const ;
%rename ( remove ) gum::Sequence::erase ( const KEY& k );
%rename ( __getitem__ ) gum::Sequence::atPos ( Idx i ) const ;
%rename ( __str__ ) gum::Sequence::toString() const;

%rename ( __str__ ) gum::DiscreteVariable::toString() const;
%rename ( __len__ ) gum::DiscreteVariable::domainSize() const;
%rename ( __getitem__ ) gum::DiscreteVariable::operator[] ( const std::string& label ) const;

%ignore gum::ListBase::insert;
%ignore gum::ListBase::pushFront;
%ignore gum::ListBase::pushBack;
%rename ( __str__ ) gum::ListBase::toString() const;
%ignore gum::List::erase ( const iterator& iter );
%ignore gum::List::erase ( const const_iterator& iter );
%ignore gum::List::eraseByVal ( const Val& val );
%ignore gum::List::eraseAllVal ( const Val& val );
%rename ( pop_front ) gum::List::popFront();
%rename ( pop_back ) gum::List::popBack();

%rename ( __str__ ) gum::DAGmodel::toString() const;
%rename ( __len__ ) gum::DAGmodel::size() const;

%rename ( __str__ ) gum::Instantiation::toString() const;
%rename ( __len__ ) gum::Instantiation::nbrDim() const;
