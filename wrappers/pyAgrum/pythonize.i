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


%define ADD_REPR_STR_API(classname)
%extend classname {
  // adding __repr__ and __str__ calling toString
  std::string __repr__() const {
   std::stringstream s;
   s<<"(classname@"<<std::hex<<self<<") "<<self->toString();
   return s.str();
  }
  std::string __str__() const { return self->toString();};
}
%enddef

ADD_REPR_STR_API(gum::Potential<double>)
ADD_REPR_STR_API(gum::Instantiation)

ADD_REPR_STR_API(gum::DiscreteVariable)
ADD_REPR_STR_API(gum::RangeVariable)
ADD_REPR_STR_API(gum::LabelizedVariable)
ADD_REPR_STR_API(gum::DiscretizedVariable<double>)

ADD_REPR_STR_API(gum::UndiGraph)
ADD_REPR_STR_API(gum::DiGraph)
ADD_REPR_STR_API(gum::MixedGraph)

ADD_REPR_STR_API(gum::BayesNet<double>)
