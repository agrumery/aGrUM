%ignore gum::MultiDimWithOffset;
%ignore gum::MultiDimImplementation;
%ignore gum::MultiDimInterface;
%ignore gum::MultiDimDecorator;
%ignore gum::MultiDimArray;

/* Synchronisation between gum::Potential and numpy array */
%pythonappend gum::Potential<double>::Potential %{
        self._notSync=True
%}
%pythonappend gum::Potential<double>::remove %{
        self._notSync=True
%}
%pythonappend gum::Potential<double>::set %{
        self._notSync=True
%}

%define CHANGE_THEN_RETURN_SELF(methodname)
%pythonappend gum::Potential<double>::methodname %{
        self._notSync=True
        return self
%}
%enddef

CHANGE_THEN_RETURN_SELF(add)
CHANGE_THEN_RETURN_SELF(abs)
CHANGE_THEN_RETURN_SELF(normalize)
CHANGE_THEN_RETURN_SELF(fillWith)
CHANGE_THEN_RETURN_SELF(sq)
CHANGE_THEN_RETURN_SELF(scale)
CHANGE_THEN_RETURN_SELF(translate)
CHANGE_THEN_RETURN_SELF(normalizeAsCPT)
CHANGE_THEN_RETURN_SELF(fillWith)
CHANGE_THEN_RETURN_SELF(set)

%rename ("$ignore", fullname=1) gum::Potential<double>::margSumOut(const Set<const DiscreteVariable*>& del_vars) const;
%rename ("$ignore", fullname=1) gum::Potential<double>::margProdOut(const Set<const DiscreteVariable*>& del_vars) const;
%rename ("$ignore", fullname=1) gum::Potential<double>::margMaxOut(const Set<const DiscreteVariable*>& del_vars) const;
%rename ("$ignore", fullname=1) gum::Potential<double>::margMinOut(const Set<const DiscreteVariable*>& del_vars) const;
%rename ("$ignore", fullname=1) gum::Potential<double>::margSumIn(const Set<const DiscreteVariable*>& kept_vars) const;
%rename ("$ignore", fullname=1) gum::Potential<double>::margProdIn(const Set<const DiscreteVariable*>& kept_vars) const;
%rename ("$ignore", fullname=1) gum::Potential<double>::margMaxIn(const Set<const DiscreteVariable*>& kept_vars) const;
%rename ("$ignore", fullname=1) gum::Potential<double>::margMinIn(const Set<const DiscreteVariable*>& kept_vars) const;

%rename ("$ignore", fullname=1) gum::Potential<double>::reorganize(const Set<const DiscreteVariable*>& vars) const;
%rename ("$ignore", fullname=1) gum::Potential<double>::putFirst(const DiscreteVariable* var) const;

%extend gum::Potential<double> {
    Potential<double> extract(PyObject* dict) {
      gum::Instantiation inst;
      PyAgrumHelper::fillInstantiationFromPyObject(self,inst,dict);
      return self->extract(inst);
    }

    Potential<double>
    reorganize(PyObject* varnames ) const {
      std::vector<const gum::DiscreteVariable*> v;
      PyAgrumHelper::fillDVVectorFromPyObject(self,v,varnames); //from helpers.h
      return self->reorganize(v);
    }

    Potential<double>
    putFirst(PyObject* varname ) const {
      const gum::DiscreteVariable* v;
      PyAgrumHelper::fillDVFromPyObject(self,v,varname); //from helpers.h
      return self->putFirst(v);
    }

    Potential<double>
    margSumOut( PyObject* varnames ) const {
      gum::Set<const gum::DiscreteVariable*> s;
      PyAgrumHelper::fillDVSetFromPyObject(self,s,varnames); //from helpers.h
      return self->margSumOut(s);
    }

    Potential<double>
    margProdOut( PyObject* varnames ) const {
      gum::Set<const gum::DiscreteVariable*> s;
      PyAgrumHelper::fillDVSetFromPyObject(self,s,varnames); //from helpers.h
      return self->margProdOut(s);
    }

    Potential<double>
    margMaxOut( PyObject* varnames ) const {
      gum::Set<const gum::DiscreteVariable*> s;
      PyAgrumHelper::fillDVSetFromPyObject(self,s,varnames); //from helpers.h
      return self->margMaxOut(s);
    }

    Potential<double>
    margMinOut( PyObject* varnames ) const {
      gum::Set<const gum::DiscreteVariable*> s;
      PyAgrumHelper::fillDVSetFromPyObject(self,s,varnames); //from helpers.h
      return self->margMinOut(s);
    }

    Potential<double>
    margSumIn( PyObject* varnames ) const {
      gum::Set<const gum::DiscreteVariable*> s;
      PyAgrumHelper::fillDVSetFromPyObject(self,s,varnames); //from helpers.h
      return self->margSumIn(s);
    }

    Potential<double>
    margProdIn( PyObject* varnames ) const {
      gum::Set<const gum::DiscreteVariable*> s;
      PyAgrumHelper::fillDVSetFromPyObject(self,s,varnames); //from helpers.h
      return self->margProdIn(s);
    }

    Potential<double>
    margMaxIn( PyObject* varnames ) const {
      gum::Set<const gum::DiscreteVariable*> s;
      PyAgrumHelper::fillDVSetFromPyObject(self,s,varnames); //from helpers.h
      return self->margMaxIn(s);
    }

    Potential<double>
    margMinIn( PyObject* varnames ) const {
      gum::Set<const gum::DiscreteVariable*> s;
      PyAgrumHelper::fillDVSetFromPyObject(self,s,varnames); //from helpers.h
      return self->margMinIn(s);
    }

    // division for python3
    gum::Potential<double> __truediv__(const gum::Potential<double>& b) {
      return *self /b;
    }

    // division for python2
    gum::Potential<double> __div__(const gum::Potential<double>& b) {
      return *self/b;
    }

    // equality
    bool __eq__(const gum::Potential<double>& b) {
      return *self==b;
    }

    // non equality
    bool __ne__(const gum::Potential<double>& b) {
      return *self!=b;
    }


  %pythoncode {

    def variablesSequence(self):
        """
        Returns
        -------
        list
            a list containing the sequence of variables
        """
        varlist = []
        for i in range(0, self.nbrDim()):
            varlist.append(self.variable(i))
        return varlist
  }
}

// copy: M indicates the modifications
%feature("shadow") gum::Potential<double>::__fill_distrib__ %{
    def __fill_distrib__(self):
      if not hasattr(self,'_notSync'):
        self._notSync=True

      if self._notSync:
        self._notSync=False
        self._var_names = []
        self._var_dims = []
        if self.empty():
            i = Instantiation(self)
            content = [self.get(i)]
            self.__distrib__ = numpy.array(content, dtype=numpy.float64) #M
            return

        content = []
        i = Instantiation(self)
        i.setFirst()
        while not i.end():
            content.append(self.get(i))
            i.inc()
        self.__distrib__ = numpy.array(content, dtype=numpy.float64) #M
        for var in self.variablesSequence():
            self._var_names.append(var.name())
            self._var_dims.append(var.domainSize())
        self._var_names.reverse()
        self._var_dims.reverse()
        self.__distrib__.shape = tuple(self._var_dims)
%}


%feature("shadow") gum::Potential::__indexfromdict__ %{
    def __indexfromdict__(self, id_dict):
        index = []
        for name, dim in zip(self._var_names, self._var_dims):
            if name in id_dict:
                id_value = id_dict[name]
                if isinstance(id_value, str):
                    # id_value is a label of a LabelizedVar
                    i = self._var_names.index(name)
                    var = self.variable(len(self._var_names) - 1 - i)
                    id_value = var[id_value]
                if id_value >= dim:
                    raise IndexError("\"%s\" size is %d !"%(name, dim))
                index.append(id_value)
            else:
                index.append(slice(None, None, None)) # equivalent to ':'
        return tuple(index)
%}

/*
* %feature("shadow") gum::Potential::__str__ %{
*     def __str__(self):
*         self.__fill_distrib__()
*         return self.__distrib__.__str__()
* %}
*/

%feature("shadow") gum::Potential::tolist %{
    def tolist(self):
        """
        Returns
        -------
        list
            the potential as a list
        """
        self.__fill_distrib__()
        return self.__distrib__.tolist()
%}

%feature("shadow") gum::Potential::toarray %{
    def toarray(self):
        """
        Returns
        -------
        array
            the potential as an array
        """
        return numpy.array(self.tolist())
%}


%feature("shadow") gum::Potential::__getitem__ %{
    def __getitem__(self, id):
        self.__fill_distrib__()
        if self.empty():
            return self.__distrib__[0]

        if isinstance(id, dict):
            id_slice = self.__indexfromdict__(id)
        else:
            id_slice = id
        return self.__distrib__[id_slice]
%}


%feature("shadow") gum::Potential::__setitem__ %{
    def __setitem__(self, id, value):
        self.__fill_distrib__()
        if self.empty():
            self.fill(value)
            self.__distrib__= numpy.array([value], dtype=numpy.float64) #M
            return

        if isinstance(id, dict):
            id_slice = self.__indexfromdict__(id)
        else:
            id_slice = id
        self.__distrib__[id_slice] = value
        self.fillWith(self.__distrib__.reshape(self.__distrib__.size).tolist())
%}


%feature("shadow") gum::Potential::var_names %{
    @property
    def var_names(self):
        """
        Returns
        -------
        list
            a list containing the name of each variables in the potential
        """
        self.__fill_distrib__()
        return self._var_names
%}


%feature("shadow") gum::Potential::var_dims %{
    @property
    def var_dims(self):
        """
        Returns
        -------
        list
            a list containing the dimensions of each variables in the potential
        """
        self.__fill_distrib__()
        return self._var_dims
%}


// these void class extensions are rewritten by "shadow" declarations
%extend gum::Potential<double> {
    void __fill_distrib__() {}
    PyObject * __indexfromdict__(PyObject *id_dict) { return NULL; }
    PyObject *tolist() { return NULL; }
    PyObject *toarray() { return NULL; }
    void __getitem__(PyObject *id) {}
    void __setitem__(PyObject *id, PyObject *value) {}
    void var_names() {}
    void var_dims() {}
}
