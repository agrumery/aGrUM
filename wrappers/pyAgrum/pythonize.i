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


%ignore gum::ListBase::insert;
%ignore gum::ListBase::pushFront;
%ignore gum::ListBase::pushBack;
%ignore gum::List::erase ( const iterator& iter );
%ignore gum::List::erase ( const const_iterator& iter );
%ignore gum::List::eraseByVal ( const Val& val );
%ignore gum::List::eraseAllVal ( const Val& val );
%rename ( pop_front ) gum::List::popFront();
%rename ( pop_back ) gum::List::popBack();

%rename ( __str__ ) *::toString() const;
