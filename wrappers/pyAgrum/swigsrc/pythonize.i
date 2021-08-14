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
%ignore classname::toString;
%enddef

ADD_REPR_STR_API(gum::Potential<double>)
ADD_REPR_STR_API(gum::Instantiation)

ADD_REPR_STR_API(gum::DiscreteVariable)
ADD_REPR_STR_API(gum::RangeVariable)
ADD_REPR_STR_API(gum::LabelizedVariable)
ADD_REPR_STR_API(gum::IntegerVariable)
ADD_REPR_STR_API(gum::DiscretizedVariable<double>)

ADD_REPR_STR_API(gum::UndiGraph)
ADD_REPR_STR_API(gum::DiGraph)
ADD_REPR_STR_API(gum::MixedGraph)

ADD_REPR_STR_API(gum::IBayesNet<double>)
ADD_REPR_STR_API(gum::BayesNet<double>)
ADD_REPR_STR_API(gum::InfluenceDiagram<double>)

ADD_REPR_STR_API(gum::credal::CredalNet<double>)

ADD_REPR_STR_API(gum::IMarkovNet<double>)
ADD_REPR_STR_API(gum::MarkovNet<double>)
ADD_REPR_STR_API(gum::MarkovNet<double>)

ADD_REPR_STR_API(gum::learning::BNLearner<double>)
