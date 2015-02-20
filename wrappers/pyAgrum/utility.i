/*
This file is just the copy of potential.i 
s/Potential/UtilityTable/g
*/

%ignore gum::MultiDimWithOffset;
%ignore gum::MultiDimImplementation;
%ignore gum::MultiDimInterface;
%ignore gum::MultiDimDecorator;
%ignore gum::MultiDimArray;

/* Synchronisation between gum::UtilityTable and numpy array */
%pythonappend gum::UtilityTable::UtilityTable %{
        self._notSync=True
%}
%pythonappend gum::UtilityTable::add %{
        self._notSync=True
%}
%pythonappend gum::UtilityTable::remove %{
        self._notSync=True
%}
%pythonappend gum::UtilityTable::set %{
        self._notSync=True
%}

%extend gum::UtilityTable<double> {
%pythoncode {
    def variablesSequence(self):
        varlist = []
        for i in range(0, self.nbrDim()):
            varlist.append(self.variable(i))
        return varlist

    def __mul__(self,p2):
        """
        return self * p2
        """
        p=UtilityTable()
        p.multiplicate(self,p2)
        return p

    def eliminates(self,var):
        """
        eliminates a variable in the UtilityTable. Returns the new UtilityTable or self if the variable is not in self.
        @warning : returns a list with only one scalar if eliminates remove the last variable
        """
        if var.name() in self.var_names:
            q=UtilityTable()
            for i in range(self.nbrDim()):
                if self.variable(i)!=var:
                    q.add(self.variable(i))
            if q.nbrDim()>0:
                q.marginalize(self)
            else:
                q=[self.sum()]
            return q
        else:
            return self
  }
}

// copy: M indicates the modifications
%feature("shadow") gum::UtilityTable<double>::__fill_distrib__ %{
    def __fill_distrib__(self):
      if not hasattr(self,'_notSync'):
        self._notSync=True

      if self._notSync:
        self._notSync=False
        self._var_names = []
        self._var_dims = []
        content = []
        if self.empty():
            self.__distrib__ = numpy.array(content, dtype=numpy.float64) #M
            return
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


%feature("shadow") gum::UtilityTable::__indexfromdict__ %{
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
* %feature("shadow") gum::UtilityTable::__str__ %{
*     def __str__(self):
*         self.__fill_distrib__()
*         return self.__distrib__.__str__()
* %}
*/

%feature("shadow") gum::UtilityTable::tolist %{
    def tolist(self):
        self.__fill_distrib__()
        return self.__distrib__.tolist()
%}

%feature("shadow") gum::UtilityTable::toarray %{
    def toarray(self):
        return numpy.array(self.tolist())
%}


%feature("shadow") gum::UtilityTable::__getitem__ %{
    def __getitem__(self, id):
        self.__fill_distrib__()
        if self.empty():
            raise IndexError("%s is empty !!"%(str(self)))
        if isinstance(id, dict):
            id_slice = self.__indexfromdict__(id)
        else:
            id_slice = id
        return self.__distrib__[id_slice]
%}


%feature("shadow") gum::UtilityTable::__setitem__ %{
    def __setitem__(self, id, value):
        self.__fill_distrib__()
        if self.empty():
            raise IndexError("%s is empty !!"%(str(self)))
        if isinstance(id, dict):
            id_slice = self.__indexfromdict__(id)
        else:
            id_slice = id
        self.__distrib__[id_slice] = value
        self.fillWith(self.__distrib__.reshape(self.__distrib__.size).tolist())
%}


%feature("shadow") gum::UtilityTable::var_names %{
    @property
    def var_names(self):
        self.__fill_distrib__()
        return self._var_names
%}


%feature("shadow") gum::UtilityTable::var_dims %{
    @property
    def var_dims(self):
        self.__fill_distrib__()
        return self._var_dims
%}


// these void class extensions are rewritten by "shadow" declarations
%extend gum::UtilityTable<double> {
    void __fill_distrib__() {}
}
%extend gum::UtilityTable<double> {
    PyObject * __indexfromdict__(PyObject *id_dict) { return NULL; }
    const char * __str__() { return NULL; }
    PyObject *tolist() { return NULL; }
    PyObject *toarray() { return NULL; }
    void __getitem__(PyObject *id) {}
    void __setitem__(PyObject *id, PyObject *value) {}
    void var_names() {}
    void var_dims() {}
}
