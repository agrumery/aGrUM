// this file is for giving access to methods defined in template ancestror.
// SWIG does not allow to use "using" for this kind of methods.

%define ADD_MULTIDIMDECORATOR_API(classname)
%extend classname {
/* wrapping the minimal interface from MultiDimDecorator */
  double get(const gum::Instantiation& i) const {
    return self->gum::MultiDimDecorator<double>::get(i);
  }

  void set ( const Instantiation& i, const double& value ) const {
    self->gum::MultiDimDecorator<double>::set(i,value);
  }

  bool empty() const {
    return self->gum::MultiDimDecorator<double>::empty();
  }


  gum::Idx pos ( const gum::DiscreteVariable& v) const {
    return self->gum::MultiDimDecorator<double>::pos(v);
  }

  bool contains ( const gum::DiscreteVariable& v) const {
    return self->gum::MultiDimDecorator<double>::contains(v);
  }

  gum::Idx nbrDim() const {
    return self->gum::MultiDimDecorator<double>::nbrDim();
  }

  const gum::DiscreteVariable& variable ( Idx i) const {
    return self->gum::MultiDimDecorator<double>::variable(i);
  }

  void fillWith ( const std::vector< double >& v ) const {
    self->gum::MultiDimDecorator<double>::fillWith(v);
  }

  void fill ( const double& d ) const {
    self->gum::MultiDimDecorator<double>::fill(d);
  }

  void remove(const gum::DiscreteVariable& var) {
    self->erase(var);
  }

  void add ( const DiscreteVariable& v ) {
    self->gum::MultiDimDecorator<double>::add(v);
  }
}
%enddef
ADD_MULTIDIMDECORATOR_API(gum::Potential<double>) 
ADD_MULTIDIMDECORATOR_API(gum::UtilityTable<double>) 


%define ADD_CREDALINFERENCEENGINCE_API(classname)
%extend classname  {
  void setRepetitiveInd(const bool flag) {
    self->gum::credal::InferenceEngine<double>::setRepetitiveInd(flag);
  }
 const std::vector< double >& marginalMax ( const NodeId id ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMax(id);
  }
 const std::vector< double >& marginalMin ( const NodeId id ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMin(id);
  }
 const std::vector< double >& marginalMax ( const std::string name ) const {
    return self->gum::credal::InferenceEngine<double>::marginalMax(name);
  }
 const std::vector< double >& marginalMin ( const std::string name) const {
    return self->gum::credal::InferenceEngine<double>::marginalMin(name);
  }
  void insertModalsFile ( const std::string& path ) {
    self->gum::credal::InferenceEngine<double>::insertModalsFile(path);
  }
  const std::vector< double >& dynamicExpMax ( const std::string& varName ) const {
    return self->gum::credal::InferenceEngine<double>::dynamicExpMax(varName);
  }
  const std::vector< double >& dynamicExpMin ( const std::string& varName ) const {
    return self->gum::credal::InferenceEngine<double>::dynamicExpMin(varName);
  }
}
%enddef
ADD_CREDALINFERENCEENGINCE_API(%arg(gum::credal::CNMonteCarloSampling<double, gum::LazyPropagation<double> >))
ADD_CREDALINFERENCEENGINCE_API(gum::credal::CNLoopyPropagation<double>)

