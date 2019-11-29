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
%ignore gum::MultiDimWithOffset;
%ignore gum::MultiDimImplementation;
%ignore gum::MultiDimInterface;
%ignore gum::MultiDimDecorator;
%ignore gum::MultiDimArray;

/* Synchronisation between gum::Potential and numpy array */
%pythonappend gum::Potential<double>::Potential %{
        self._notSync=True
        self._list_vars=list()
%}
%pythonappend gum::Potential<double>::remove %{
        self._list_vars.remove(var)
        self._notSync=True
%}
%pythonappend gum::Potential<double>::set %{
        self._notSync=True
%}

%pythonappend gum::Potential<double>::add %{
        self._list_vars.append(v)
        self._notSync=True
        return self
%}


%define CHANGE_THEN_RETURN_SELF(methodname)
%pythonappend gum::Potential<double>::methodname %{
        self._notSync=True
        return self
%}
%enddef

CHANGE_THEN_RETURN_SELF(abs)
CHANGE_THEN_RETURN_SELF(normalize)
CHANGE_THEN_RETURN_SELF(fillWith)
CHANGE_THEN_RETURN_SELF(sq)
CHANGE_THEN_RETURN_SELF(scale)
CHANGE_THEN_RETURN_SELF(translate)
CHANGE_THEN_RETURN_SELF(normalizeAsCPT)

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

/* Convert from C --> Python */
%typemap(out) gum::Set<gum::Instantiation> {
    $result = PyAgrumHelper::PySeqFromSetOfInstantiation($1);
}

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
    
    def loopIn(self):
      """
      Generator to iterate inside a Potential. 

      Yield an gum.Instantiation that iterates over all the possible values for the gum.Potential

      Examples
      --------
      >>> import pyAgrum as gum
      >>> bn=gum.fastBN("A[3]->B[3]<-C[3]")
      >>> for i in bn.cpt("B").loopIn():
            print(i)
            print(bn.cpt("B").get(i))
            bn.cpt("B").set(i,0.3) 
      """
      i=Instantiation(self)
      i.setFirst()
      while not i.end():
        yield i
        i.inc()
      return

    def fillWithFunction(self,s,noise=None):
      """
      Automatically fills the potential as a (quasi) deterministic CPT with the evaluation of the expression s.

      The expression s gives a value for the first variable using the names of the last variables.
      The computed CPT is deterministic unless noise is used to add a 'probabilistic' noise around the exact value given by the expression.


      Examples
      --------
      >>> import pyAgrum as gum
      >>> bn=gum.fastBN("A[3]->B[3]<-C[3]")
      >>> bn.cpt("B").fillWithFunction("(A+C)/2")

      Parameters
      ----------
      s : str
          an expression using the name of the last variables of the Potential and giving a value to the first variable of the Potential
      noise : list
          an (odd) list of numerics giving a pattern of 'probabilistic noise' around the value.

      Warning
      -------
          The expression may have any numerical values, but will be then transformed to the closest correct value for the range of the variable.

      Returns
      -------
      pyAgrum.Potential
            a reference to the modified potential

      Raises
      ------
      gum.InvalidArgument
        If the first variable is Labelized or if the len of the noise is not odd.
      """
      if self.variable(0).varType()==VarType_Labelized:
        raise InvalidArgument("[pyAgrum] The variable "+self.variable(0).name()+" is a LabelizedVariable")

      if noise==None:
        mid=0
      else:
        if len(noise)%2==0:
          raise InvalidArgument("[pyAgrum] len(noise) must not be even")
        mid=int((len(noise)-1)/2)

      self.fillWith(0)
      mi=self.variable(0).numerical(0)
      ma=self.variable(0).numerical(self.variable(0).domainSize()-1)

      I=Instantiation(self)

      I.setFirst()
      while not I.end():
        vars={self.variable(i).name():self.variable(i).numerical(I.val(i)) for i in range(1,self.nbrDim())}
        res=eval(s,None,vars)
        if res<mi:
          res=mi
        if res>ma:
          res=ma
        pos=self.variable(0).index(str(res))
        if mid==0:
          I.chgVal(0,pos)
          self.set(I,1)
        else:
          for i,v in enumerate(noise):
            if 0<=pos+i-mid<self.variable(0).domainSize():
              I.chgVal(0,pos+i-mid)
              self.set(I,v)
        I.incNotVar(self.variable(0))
      self.normalizeAsCPT()
      return self

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

        Warnings
        --------
            Listed in reverse from the variable enumeration order
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
