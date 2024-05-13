/**
 *
 *  Copyright 2005-2023 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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


/* keep tracks of variables to trick with  garbage collector */
%pythonappend gum::Potential<double>::Potential %{
        self._list_vars=list()
%}
%pythonappend gum::Potential<double>::remove %{
        self._list_vars.remove(var)
%}

%pythonappend gum::Potential<double>::add %{
        self._list_vars.append(v)
        return self
%}


%define CHANGE_THEN_RETURN_SELF(methodname)
%pythonappend gum::Potential<double>::methodname %{
        return self
%}
%enddef

CHANGE_THEN_RETURN_SELF(abs)
CHANGE_THEN_RETURN_SELF(sq)
CHANGE_THEN_RETURN_SELF(log2)
CHANGE_THEN_RETURN_SELF(sgn)
CHANGE_THEN_RETURN_SELF(normalize)
CHANGE_THEN_RETURN_SELF(normalizeAsCPT)
CHANGE_THEN_RETURN_SELF(scale)
CHANGE_THEN_RETURN_SELF(inverse)
CHANGE_THEN_RETURN_SELF(translate)

CHANGE_THEN_RETURN_SELF(fillWith)
%pythonprepend gum::Potential<double>::fillWith %{
if len(args)>1:
  d=args[1]
  if type(d)==dict:
    if set(d.keys())==set(self.names):
      return self.fillWith(args[0],[d[s] for s in self.names])
    else:
      raise pyAgrum.ArgumentError(f"[pyAgrum] keys in dict {tuple(d.keys())} does not match the Potential's variables {self.names}")
%}

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

%rename ("$ignore", fullname=1) gum::Potential<double>::argmin() const;
%rename ("$ignore", fullname=1) gum::Potential<double>::argmax() const;


%extend gum::Potential<double> {
  PyObject *expectedValue(PyObject* pyfunc) const {
    if (!PyCallable_Check(pyfunc)) { PyErr_SetString(PyExc_TypeError, "Need a callable object!"); }
    double res=self->expectedValue([&](const gum::Instantiation& i) -> double {
      double val;
      PyObject* arg=PyAgrumHelper::instantiationToDict(i,false);
      PyObject* args=PyTuple_New(1);
      PyTuple_SetItem(args,0,arg);
      PyObject* res=PyObject_Call(pyfunc,args,NULL);
      Py_DecRef(args);
      Py_DecRef(arg);

      if (res==NULL) {
        val=0;
      } else {
        val=PyFloat_AsDouble(res);
        Py_DecRef(res);
      }
      return val;
      });
    return PyFloat_FromDouble(res);
  }

  Potential<double> extract(PyObject* arg) {
    if (PyDict_Check(arg)) {
      gum::Instantiation inst;
      PyAgrumHelper::fillInstantiationFromPyObject(self, inst, arg);
      return self->extract(inst);
    } else {
      //auto arg1 = reinterpret_cast< gum::Instantiation * >(arg);
      //if (arg1==nullptr) {
        GUM_ERROR(gum::InvalidArgument,"arg is neither a dict or an pyAgrum.Instantiation.");
      //}
      //return self->extract(*arg1);
    }
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

    PyObject* argmin() {
      const auto [argmi,mi] = self->argmin();
      return PyTuple_Pack(2,PyAgrumHelper::PySeqFromSetOfInstantiation(argmi),PyFloat_FromDouble(mi));
    }

    PyObject* argmax() {
      const auto [argma,ma] = self->argmax();
      return PyTuple_Pack(2,PyAgrumHelper::PySeqFromSetOfInstantiation(argma),PyFloat_FromDouble(ma));
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
    def __radd__(self,other):
      return self.__add__(other)

    def __rmul__(self,other):
      return self.__mul__(other)

    def __rsub__(self,other):
      return (self*-1)+other

    def __rfloordiv__(self,other):
      return Potential(self).inverse().scale(other)

    def __rtruediv__(self,other):
      return Potential(self).inverse().scale(other)

    def __rdiv__(self,other):
      return Potential(self).inverse().scale(other)

    def __neg__(self):
      return -1*self

    def __abs__(self):
      return Potential(self).abs()

    def loopIn(self):
      """
      Generator to iterate inside a Potential.

      Yield an pyAgrum.Instantiation that iterates over all the possible values for the pyAgrum.Potential

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

      Warning
      -------
          The expression may have any numerical values, but will be then transformed to the closest correct value for the range of the variable.

      Note
      ----
          Deprecated. Please use pyAgrum.Potential.fillFromFunction instead.

      Returns
      -------
      pyAgrum.Potential
            a reference to the modified potential

      Raises
      ------
        pyAgrum.InvalidArgument
          If the first variable is Labelized or Integer, or if the len of the noise is not odd.
      """

      warnings.warn("""
        ** pyAgrum.fillWithFunction is deprecated from pyAgrum>1.12.1. Please use pyAgrum.fillFromFunction instead (Noise is not used anymore).
      """, DeprecationWarning, stacklevel=2)
      return self.fillFromFunction(s)


    def fillFromExpression(self,s_fn):
      """
      Automatically fills the potential with the evaluation of the expression s_fn (no matter if is a CPT or not).

      The symbolic expression s_fn gives a value for each parameters of the Potential

      Examples
      --------
      >>> import pyAgrum as gum
      >>> bn=pyAgrum.fastBN('A[3]->B[3]<-C[3]')
      >>> bn.cpt('B').fillFromFunction('(B+A+C)/2')

      Parameters
      ----------
      s_fn : str
          a symbolic expression using the name of the variables of the Potential and giving a value to the first variable of the Potential. This evaluation is done in a context that inclides 'math' module.

      Warning
      -------
          The expression may have any numerical values, but will be then transformed to the closest correct value for the range of the variable.


      Returns
      -------
      pyAgrum.Potential
            a reference to the modified potential
      """
      import math
      forbidden=frozenset(['__import__','__class__'])

      I=pyAgrum.Instantiation(self)
      code=float(s_fn) if isinstance(s_fn, (int, float)) else compile(s_fn,"<string>","eval")
      if not isinstance(s_fn, (int, float)):
        if forbidden & set(code.co_names):
          raise InvalidArgument("[pyAgrum] '__import__' is not allowed in the expression '"+s_fn+"'")

      I.setFirst()
      while not I.end():
        vars={self.variable(i).name():self.variable(i).numerical(I.val(i)) for i in range(self.nbrDim())}
        res=s_fn if isinstance(s_fn, (int, float)) else eval(code,{'math':math},vars)        
        self.set(I,res)
        I.inc()

      return self

    def fillFromFunction(self,s_fn):
      """
      Automatically fills the potential as a deterministic CPT with the evaluation of the expression s_fn.

      The symbolic expression s_fn gives a value for the first variable, depending on the following variables.
      The computed CPT is deterministic.

      Examples
      --------
      >>> import pyAgrum as gum
      >>> bn=gum.fastBN('A[3]->B[3]<-C[3]')
      >>> bn.cpt('B').fillFromFunction('(A+C)/2')

      Parameters
      ----------
      s_fn : str
          a symbolic expression using the name of the second and following variables of the Potential and giving a value to the first variable of the Potential. This evaluation is done in a context that inclides 'math' module.

      Warning
      -------
          The expression may have any numerical values, but will be then transformed to the closest correct value for the range of the variable.


      Returns
      -------
      pyAgrum.Potential
            a reference to the modified potential

      Raises
      ------
        pyAgrum.InvalidArgument
          If the first variable is Labelized.
      """
      import math
      forbidden=frozenset(['__import__','__class__'])

      if self.variable(0).varType()== gum.VarType_Labelized:
        raise InvalidArgument("[pyAgrum] The variable "+self.variable(0).name()+" is a LabelizedVariable")

      self.fillWith(0)
      I=gum.Instantiation(self)
      code=float(s_fn) if isinstance(s_fn, (int, float)) else compile(s_fn,"<string>","eval")
      if not isinstance(s_fn, (int, float)):
        if forbidden & set(code.co_names):
          raise InvalidArgument("[pyAgrum] '__import__' is not allowed in the expression '"+s_fn+"'")

      I.setFirst()
      while not I.end():
        vars={self.variable(i).name():self.variable(i).numerical(I.val(i)) for i in range(1,self.nbrDim())}
        res=s_fn if isinstance(s_fn, (int, float)) else eval(code,{'math':math},vars)
        pos=self.variable(0).closestIndex(res)
        I.chgVal(0,pos)
        self.set(I,1)
        I.incNotVar(self.variable(0))

      self.normalizeAsCPT()
      return self


    def fillFromDistribution(self,distribution,**s_fns):
      """
      Automatically fills the potential as a familly of distributions whose parameters are found using evaluation of the expressions s_fns.

      The symbolic expressions s_fns gives a value for the named parameters of the distributions.

      Examples
      --------
      >>> import pyAgrum as gum
      >>> bn=gum.fastBN('A[3]->B[3]<-C[3]')
      >>> bn.cpt('B').fillFromFunction('(A+C)/2')

      Parameters
      ----------
      s_fns : a list of named arguments (str)
          the named arguments with an evaluation of the expressions in s_fns are passed as argument for the chosen distribution.

      Returns
      -------
      pyAgrum.Potential
            a reference to the modified potential

      Raises
      ------
        pyAgrum.InvalidArgument
          If the first variable is Labelized.
      """
      import math
      forbidden=frozenset(['__import__','__class__'])

      var=self.variable(0)
      if var.varType()== gum.VarType_Labelized:
        raise InvalidArgument("[pyAgrum] The variable "+self.variable(0).name()+" is a LabelizedVariable")

      codes={k:float(s_fns[k]) if isinstance(s_fns[k], (int, float)) else compile(s_fns[k],"<string>","eval") for k in s_fns.keys()}
      for _,code in codes.items():
        if not isinstance(code, (int, float)):
          if forbidden & set(code.co_names):
            raise InvalidArgument("[pyAgrum] '__import__' is not allowed in the expression '"+code+"'")

      I=gum.Instantiation()
      for i in range(1,self.nbrDim()):
        I.add(self.variable(i))

      if hasattr(distribution,'pdf'):
        d=distribution.pdf
      elif hasattr(distribution,'pmf'):
        d=distribution.pmf
      else:
        raise InvalidArgument("[pyAgrum] The distribution must have a pdf or a pmf method")
      I.setFirst()
      vals=[var.numerical(i) for i in range(var.domainSize())]
      while not I.end():
        vars={self.variable(i).name():self.variable(i).numerical(I.val(i-1)) for i in range(1,self.nbrDim())}
        args={k:float(s_fns[k]) if isinstance(s_fns[k], (int, float)) else eval(codes[k],{'math':math},vars) for k in s_fns.keys()}
        di=I.todict()
        self[di]=d(vals,**args)
        I.inc()
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

    def __prepareIndices__(self,ind):
      """
      From an indice (dict or tuple), returns a pair of pyAgrum.Instantiation to loop in a part of the Potential.
      """
      loopvars=Instantiation(self)
      loopvars.setMutable()

      inst=Instantiation(self)
      inst.setFirst()

      if isinstance(ind, (Number,slice)):
        i = tuple([ind])
      else:
        i = ind

      if isinstance(i,dict):
          for nam in self.names:
              if nam in i:
                  inst.chgVal(nam,i[nam])
                  loopvars.erase(nam)
      elif isinstance(i,tuple):
          vn=[n for n in reversed(self.names)]
          if len(i)>self.nbrDim():
              raise KeyError("Too many values in '"+str(i)+"' for '"+str(self)+"'")
          for k,v in enumerate(i):
              if not isinstance(v,slice):
                  nam=vn[k]
                  inst.chgVal(nam,v)
                  loopvars.erase(nam)
      else:
          raise ValueError("No subscript using '"+str(i)+"'")
      return inst,loopvars

    def __getitem__(self, id):
      if isinstance(id,Instantiation):
          return self.get(id)

      inst,loopvars=self.__prepareIndices__(id)

      if loopvars.nbrDim()==0:
          return self.get(inst)

      if loopvars.nbrDim()==self.nbrDim():
        content=[]

        inst=Instantiation(self)
        while not inst.end():
            content.append(self.get(inst))
            inst.inc()
        tab=numpy.array(content,dtype=numpy.float64)
        tab.shape=tuple(reversed(self.shape))
        return tab

      names=[loopvars.variable(i-1).name() for i in range(loopvars.nbrDim(),0,-1)]
      tab=numpy.zeros(tuple([loopvars.variable(i-1).domainSize() for i in range(loopvars.nbrDim(),0,-1)]))
      while not inst.end():
          indice=[inst.val(name) for name in names]
          tab[tuple(indice)]=self.get(inst)
          inst.incIn(loopvars)
      return tab

    def __setitem__(self, id, value):
      if isinstance(id,Instantiation):
          self.set(id,value)
          return

      inst,loopvars=self.__prepareIndices__(id)

      if loopvars.nbrDim()==0:
          self.set(inst,value)
          return

      if isinstance(value,Number):
        while not inst.end():
            self.set(inst,value)
            inst.incIn(loopvars)
      else:
        if isinstance(value,list):
            value=numpy.array(value)
        elif isinstance(value,dict):
            if loopvars.nbrDim()>1:
                raise ArgumentError("The value can be a dict only when specifying 1D-marginal.")
            var=loopvars.variable(0)
            for label in var.labels():
                if label not in value:
                  raise ArgumentError(f"The label '{label}' can not be found in the value.")
            for label in value.keys():
                if not var.isLabel(label):
                  raise ArgumentError(f"The label '{label}' can not be found in variable {var}.")
            value=numpy.array([value[item] for item in var.labels()])
        else:
            if not isinstance(value,numpy.ndarray):
                raise ArgumentError(f"{value} is not a correct value for a potential.")

        shape=tuple([loopvars.variable(i-1).domainSize() for i in range(loopvars.nbrDim(),0,-1)])
        if value.shape!=shape:
          raise IndexError("Shape of '"+str(value)+"' is not '"+str(shape)+"'")

        names = [loopvars.variable(i - 1).name() for i in range(loopvars.nbrDim(), 0, -1)]
        while not inst.end():
            indice = tuple([inst.val(name) for name in names])
            self.set(inst,float(value[indice]))
            inst.incIn(loopvars)

    def __iter__(self):
        """
        Iterate over the data of the Potential

        Yield
        -----
        Tuple[pyAgrum.Instantiation,float]
          The instantiation and the value in the Potential
        """
        for i in self.loopIn():
            yield i,self.get(i)

    def tolist(self):
        """
        Returns
        -------
        list
            the potential as a list
        """
        return self.__getitem__({}).tolist()

    def toarray(self):
        """
        Returns
        -------
        array
            the potential as an array
        """
        return self.__getitem__({})

    def topandas(self):
        """
        Returns
        -------
        pandas.DataFrame
           the potential as an pandas.DataFrame
        """
        import pandas as pd
        varnames = list(reversed(self.names))
        data = []
        pname = ""
        for inst in self.loopIn():
            d = {k:v for k,v in reversed(inst.todict(True).items())}
            d[pname] = self.get(inst)
            d[pname], d[varnames[-1]] = d[varnames[-1]], d[pname]
            data.append(d)
        cols = varnames[:-1] + [pname]
        return pd.DataFrame(data).set_index(cols).unstack(pname)

    def tolatex(self):
        """
        Render object to a LaTeX tabular.

        Requires to include `booktabs` package in the LaTeX document.

        Returns
        -------
        str
         the potential as LaTeX string
        """
        return self.topandas().to_latex()

    def toclipboard(self,**kwargs):
        """
        Write a text representation of object to the system clipboard. This can be pasted into spreadsheet, for instance.
        """
        return self.topandas().to_clipboard()

    @property
    def var_names(self):
        """
        Returns
        -------
        list
            a list containing the name of each variables in the potential

        Warnings
        --------
            This methods is deprecated. Please use gum.Potential.names and note the change in the order !

            var_names return a list in the reverse order of the enumeration order of the variables.
        """
        warnings.warn("\n** pyAgrum.Potential.var_names is obsolete in pyAgrum>0.22.9. Please use pyAgrum.Potential.names.\n")
        return [n for n in reversed(self.names)]

    @property
    def var_dims(self):
        """
        Returns
        -------
        list
            a list containing the dimensions of each variables in the potential

        Warnings
        --------
            This methods is deprecated. Please use gum.Potential.shape and note the change in the order !

            var_dims return a list in the reverse order of the enumeration order of the variables.
        """
        warnings.warn("\n** pyAgrum.Potential.var_dims is obsolete in pyAgrum>0.22.9. Please use pyAgrum.Potential.shape.\n")
        return [n for n in reversed(self.shape)]

    @property
    def names(self):
        """
        Returns
        -------
        list
            a list containing the name of each variables in the potential

        Warnings
        --------
            listed in the reverse order of the enumeration order of the variables.
        """
        return tuple([self.variable(i).name() for i in range(self.nbrDim())])

    @property
    def shape(self):
        """
        Returns
        -------
        list
            a list containing the dimensions of each variables in the potential

        Warnings
        --------
            `p.shape` and `p[:].shape` list the dimensions in different order
        """
        return tuple([self.variable(i).domainSize() for i in range(self.nbrDim())])
  }
}
