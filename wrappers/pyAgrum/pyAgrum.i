%define DOCSTRING
"pyAgrum is a Python wrapper for the C++ aGrUM library. It provides a high-level
interface to the part of agrum allowing to create, handle and make computations
into Bayesian Networks. The module is a straightforward application of the SWIG
interface generator. Custom-written code was added to make the interface similar
to OpenBayes, a free Bayesian Network library for Python."
%enddef
%module(docstring=DOCSTRING, directors="1") pyAgrum
%feature("autodoc", "1");


%pythoncode %{
import numpy
%}

/* EXCEPTION HANDLING */
%exception {
    try {
        $action
    } catch(std::bad_cast &e) {
        PyErr_SetString(PyExc_RuntimeError, "C++ Bad Cast");
        SWIG_fail;
    } catch(gum::NotFound &e) {
        PyErr_SetString(PyExc_IndexError, e.content().c_str());
        SWIG_fail;
    } catch(gum::DefaultInLabel &e) {
        gum::DefaultInLabel *ecopy = new gum::DefaultInLabel(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__DefaultInLabel,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__DefaultInLabel), err);
        SWIG_fail;
    } catch(gum::IdError &e) {
        gum::IdError *ecopy = new gum::IdError(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__IdError,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__IdError), err);
        SWIG_fail;
    } catch(gum::OperationNotAllowed &e) {
        gum::OperationNotAllowed *ecopy = new gum::OperationNotAllowed(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__OperationNotAllowed,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__OperationNotAllowed),
                                      err);
        SWIG_fail;
    } catch(gum::InvalidCircuit &e) {
        gum::InvalidCircuit *ecopy = new gum::InvalidCircuit(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__InvalidCircuit,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__InvalidCircuit), err);
        SWIG_fail;
    } catch(gum::InvalidNode &e) {
        gum::InvalidNode *ecopy = new gum::InvalidNode(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__InvalidNode,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__InvalidNode), err);
        SWIG_fail;
    } catch(gum::InvalidEdge &e) {
        gum::InvalidEdge *ecopy = new gum::InvalidEdge(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__InvalidEdge,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__InvalidEdge), err);
        SWIG_fail;
    } catch(gum::DuplicateElement &e) {
        gum::DuplicateElement *ecopy = new gum::DuplicateElement(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__DuplicateElement,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__DuplicateElement), err);
        SWIG_fail;
    } catch(gum::OutOfLowerBound &e) {
        gum::OutOfLowerBound *ecopy = new gum::OutOfLowerBound(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__OutOfLowerBound,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__OutOfLowerBound), err);
        SWIG_fail;
    } catch(gum::OutOfUpperBound &e) {
        gum::OutOfUpperBound *ecopy = new gum::OutOfUpperBound(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__OutOfUpperBound,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__OutOfUpperBound), err);
        SWIG_fail;
    } catch(gum::OutOfBounds &e) {
        gum::OutOfBounds *ecopy = new gum::OutOfBounds(e);
        PyObject *err = SWIG_NewPointerObj(ecopy,
                                           SWIGTYPE_p_gum__OutOfBounds,
                                           1);
        PyErr_SetObject(
            SWIG_Python_ExceptionType(SWIGTYPE_p_gum__OutOfBounds), err);
        SWIG_fail;
    } catch(gum::Exception& e) {
        PyErr_SetString(PyExc_Exception, e.content().c_str());
        SWIG_fail;
    } catch(std::exception& e) {
        PyErr_SetString(PyExc_Exception, e.what());
        SWIG_fail;
    }
}



/* MAKE OPERATIONS MORE PYTHONIC */
%ignore gum::operator<<;
%ignore *::operator=;
%ignore *::operator++;
%ignore *::operator--;
%ignore *::operator[];

%ignore gum::Sequence::operator<<;
%ignore gum::Sequence::operator>>;
%ignore gum::Sequence::operator[];

%rename(append) gum::Sequence::insert( const KEY& k );
%rename(index) gum::Sequence::pos( const KEY& key ) const ;
%rename(remove) gum::Sequence::erase( const KEY& k );
%rename(__getitem__) gum::Sequence::atPos( const Idx i ) const ;
%rename(__str__) gum::Sequence::toString() const;

%rename(__str__) gum::DiscreteVariable::toString() const;
%rename(__len__) gum::DiscreteVariable::domainSize() const;
%rename(__getitem__) gum::DiscreteVariable::operator[](const std::string& label) const;
%extend gum::DiscretizedVariable { //XXX hack, rename will be better
    gum::Idx index(const float& aTarget) const {
        return (*self)[aTarget];
    }
}
%rename(DiscreteVar) gum::DiscreteVariable;
%rename(LabelizedVar) gum::LabelizedVariable;
%rename(RangeVar) gum::RangeVariable;

%ignore gum::BayesNet::addVariable;
%ignore gum::BayesNet::eraseVariable;
%rename(__str__) gum::AbstractBayesNet::toString() const;
%rename(__len__) gum::AbstractBayesNet::size() const;

%ignore gum::ListBase::insert;
%ignore gum::ListBase::pushFront;
%ignore gum::ListBase::pushBack;
%rename(__str__) gum::ListBase::toString() const;
%ignore gum::List::erase( const iterator& iter );
%ignore gum::List::erase( const const_iterator& iter );
%ignore gum::List::eraseByVal( const Val& val );
%ignore gum::List::eraseAllVal( const Val& val );
%rename(pop_front) gum::List::popFront();
%rename(pop_back) gum::List::popBack();

%ignore gum::Instantiation::assign_values;
%rename(__str__) gum::Instantiation::toString() const;
%rename(__len__) gum::Instantiation::nbrDim() const;

%rename(remove) gum::MultiDimDecorator::erase( const DiscreteVariable& var );

/* CLASS EXTENSIONS */
%extend gum::Exception {
    const char * __str__()
    {
        return self->content().c_str();
    }
}


/* remove __disown__() since it causes an error (str has no __disown__ method)...
%pythonprepend gum::LabelizedVariable::addLabel %{
        args = tuple([args[0].__disown__()]) + args[1:]
%}
%pythonprepend gum::DiscretizedVariable::addTick %{
        args = tuple([args[0].__disown__()]) + args[1:]
%}
*/
%feature("shadow") gum::LabelizedVariable::addLabel(const std::string aLabel) %{
def addLabel(self,*args):
  $action(self,*args)
  return self
%}

%feature("shadow") gum::DiscretizedVariable<float>::addTick(const float& aTick) %{
def addTick(self,*args):
  $action(self,*args)
  return self
%}

%pythoncode %{
def availableBNExts():
  return "bif|dsl|net|bifxml"

def loadBN(s,listeners=None):
  bn=BayesNet()

  extension=s.split('.')[-1].upper()
  if extension=="BIF":
    bn.loadBIF(s,listeners) 
  elif extension=="BIFXML":
    bn.loadBIFXML(s,listeners)
  elif extension=="DSL":
    bn.loadDSL(s,listeners)
  elif extension=="NET":
    bn.loadNET(s,listeners)
  else:
    raise Exception("extension "+s.split('.')[-1]+" unknown. Please use "+availableBNExts())

  bn.setProperty("name",s)
  return bn

def saveBN(bn,s):
  extension=s.split('.')[-1].upper()
  if extension=="BIF":
    bn.saveBIF(s)
  elif extension=="BIFXML":
    bn.saveBIFXML(s)
  elif extension=="DSL":
    bn.saveDSL(s)
  elif extension=="NET":
    bn.saveNET(s)
  else:
    raise Exception("extension "+s.split('.')[-1]+" unknown. Please use "+availableBNExts())


%}

%extend gum::BayesNet {
    PyObject *pyTopologicalOrder() const {        
        PyObject* q=PyList_New(0);

        gum::Sequence<gum::NodeId> topoOrder=self->topologicalOrder();
        for(gum::Idx i=0;i<topoOrder.size();i++) {
		PyList_Append(q,PyInt_FromLong(topoOrder.atPos(i)));
        }

        return q;
    };

    PyObject *names() const {
		PyObject* q=PyList_New(0);
	
		const DAG& dag=self->dag();
		for ( NodeGraphPartIterator node_iter = dag.beginNodes();node_iter != dag.endNodes(); ++node_iter ) {
			PyList_Append(q,PyString_FromString(self->variable(*node_iter).name().c_str()));
		}

		return q;
	};

    PyObject *ids() {
		PyObject* q=PyList_New(0);

		const DAG& dag=self->dag();
		for ( NodeGraphPartIterator  node_iter = dag.beginNodes();node_iter != dag.endNodes(); ++node_iter ) {
			PyList_Append(q,PyInt_FromLong(*node_iter));
		}

		return q;
	};

    PyObject *parents(const NodeId id) const {
		PyObject* q=PyList_New(0);

		const NodeSet& p=self->dag().parents(id);
		for(NodeSet::const_iterator it=p.begin();it!=p.end();++it) {
			PyList_Append(q,PyInt_FromLong(*it));
		}

		return q;
	};

    PyObject *children(const NodeId id) const {
		PyObject* q=PyList_New(0);

		const NodeSet& p=self->dag().children(id);
		for(NodeSet::const_iterator it=p.begin();it!=p.end();++it) {
			PyList_Append(q,PyInt_FromLong(*it));
		}

		return q;
	};

    bool loadBIF(std::string name, PyObject *l=(PyObject*)0)
    {
    	  std::vector<PythonLoadListener> py_listener;
        try {
            gum::BIFReader<T_DATA> reader(self,name);
            int l_size=__fillListeners(py_listener,l);
            for(int i=0 ; i<l_size ; i++) {
                GUM_CONNECT(reader.scanner(), onLoad, py_listener[i], PythonLoadListener::whenLoading);
            }

            if (! reader.proceed()) {
                reader.showElegantErrorsAndWarnings();
                reader.showErrorCounts();
                return false;
            } else {
                return true;
            }
        } catch (gum::IOError& e) {
          GUM_SHOWERROR(e);
        }
        return false;
    }

    void saveBIF(std::string name) {
        gum::BIFWriter<T_DATA> writer;
        writer.write( name, *self );
    }

    bool loadDSL(std::string name, PyObject *l=(PyObject*)0)
    {
	std::vector<PythonLoadListener> py_listener;
        try {
            gum::DSLReader<T_DATA> reader(self,name);
            int l_size=__fillListeners(py_listener,l);
            for(int i=0 ; i<l_size ; i++) {
                GUM_CONNECT(reader.scanner(), onLoad, py_listener[i], PythonLoadListener::whenLoading);
            }

            if (! reader.proceed()) {
                reader.showElegantErrorsAndWarnings();
                reader.showErrorCounts();
                return false;
            } else {
                return true;
            }
        } catch (gum::IOError& e) {GUM_SHOWERROR(e);}
        return false;
    }

    void saveDSL(std::string name) {
        gum::DSLWriter<T_DATA> writer;
        writer.write( name, *self );
    }

    bool loadNET(std::string name, PyObject *l=(PyObject*)0)
    {
        std::vector<PythonLoadListener> py_listener;
        try {
            gum::NetReader<T_DATA> reader(self,name);
            int l_size=__fillListeners(py_listener,l);
            for(int i=0 ; i<l_size ; i++) {
                GUM_CONNECT(reader.scanner(), onLoad, py_listener[i], PythonLoadListener::whenLoading);
            }

            if (! reader.proceed()) {
                reader.showElegantErrorsAndWarnings();
                reader.showErrorCounts();
                return false;
            } else {
                return true;
            }
        } catch (gum::IOError& e) {GUM_SHOWERROR(e);}
        return false;
    }

   void saveNET(std::string name) {
        gum::NetWriter<T_DATA> writer;
        writer.write( name, *self );
    }

   bool loadBIFXML(std::string name, PyObject *l=(PyObject*)0)
    {
        try {
            gum::BIFXMLBNReader<T_DATA> reader(self,name);
            /* nothing as listener for now for BIFXML ... */
            /*            
            int l_size=__fillListeners(py_listener,l);
            for(int i=0 ; i<l_size ; i++) {
                GUM_CONNECT(reader.scanner(), onLoad, py_listener[i], PythonLoadListener::whenLoading);
            }
            */

            int isOK= reader.proceed();
            return (isOK==0);
        } catch (gum::IOError& e) {
            GUM_SHOWERROR(e);
        }

        return false;
   }

    void saveBIFXML(std::string name) {
        gum::BIFXMLBNWriter<T_DATA> writer;
        writer.write( name, *self );
    }
}

%feature("shadow") gum::MultiDimDecorator::variablesSequence %{
def variablesSequence(self):
    varlist = []
    for i in range(0, self.nbrDim()):
        varlist.append(self.variable(i))
    return varlist
%}


%pythonappend gum::BayesNet::cpt %{
        val.__fill_distrib__()
%}


%pythonappend gum::Potential::Potential %{
        self.__fill_distrib__()
%}


%pythonappend gum::MultiDimDecorator::add %{
        self.__fill_distrib__()
%}


%feature("shadow") gum::Potential<float>::__fill_distrib__ %{
    def __fill_distrib__(self):
        self._var_names = []
        self._var_dims = []
        content = []
        if self.empty():
            self.__distrib__ = numpy.array(content, dtype=float32)
            return
        i = Instantiation(self)
        i.setFirst
        while not i.end():
            content.append(self.get(i))
            i.inc()
        self.__distrib__ = numpy.array(content, dtype=float32)
        for var in self.variablesSequence():
            self._var_names.append(var.name())
            self._var_dims.append(len(var))
        self._var_names.reverse()
        self._var_dims.reverse()
        self.__distrib__.shape = tuple(self._var_dims)
%}


// copy: M indicates the modifications
%feature("shadow") gum::Potential<double>::__fill_distrib__ %{
    def __fill_distrib__(self):
        self._var_names = []
        self._var_dims = []
        content = []
        if self.empty():
            self.__distrib__ = numpy.array(content, dtype=numpy.float64) #M
            return
        i = Instantiation(self)
        i.setFirst
        while not i.end():
            content.append(self.get(i))
            i.inc()
        self.__distrib__ = numpy.array(content, dtype=numpy.float64) #M
        for var in self.variablesSequence():
            self._var_names.append(var.name())
            self._var_dims.append(len(var))
        self._var_names.reverse()
        self._var_dims.reverse()
        self.__distrib__.shape = tuple(self._var_dims)
%}


%feature("shadow") gum::Potential::__indexfromdict__ %{
    def __indexfromdict__(self, id_dict):
        index = []
        #for id in id_dict:
        #    if not id in self._var_names:
        #        raise IndexError("\"%s\" is not a variable of this table !"
        #                         %(id))
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


%feature("shadow") gum::Potential::__str__ %{
    def __str__(self):
        self.__fill_distrib__()
        return self.__distrib__.__str__()
%}


%feature("shadow") gum::Potential::tolist %{
    def tolist(self):
        self.__fill_distrib__()
        return self.__distrib__.tolist()
%}

%feature("shadow") gum::Potential::toarray %{
    def toarray(self):
        return numpy.array(self.tolist())
%}


%feature("shadow") gum::Potential::__getitem__ %{
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


%feature("shadow") gum::Potential::__setitem__ %{
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


%feature("shadow") gum::Potential::var_names %{
    @property
    def var_names(self):
        self.__fill_distrib__()
        return self._var_names
%}


%feature("shadow") gum::Potential::var_dims %{
    @property
    def var_dims(self):
        self.__fill_distrib__()
        return self._var_dims
%}


// these void class extensions are rewritten by "shadow" declarations
%extend gum::Potential<float> {
    void __fill_distrib__() {}
}
%extend gum::Potential<double> {
    void __fill_distrib__() {}
}
%extend gum::Potential {
    PyObject * __indexfromdict__(PyObject *id_dict) { return NULL; }
    const char * __str__() { return NULL; }
    PyObject *tolist() { return NULL; }
    PyObject *toarray() { return NULL; }
    void __getitem__(PyObject *id) {}
    void __setitem__(PyObject *id, PyObject *value) {}
    void var_names() {}
    void var_dims() {}
}


%pythonprepend gum::List::append %{
        args[0].__disown__() #lets c++ take care of deleting it
%}


%pythonappend gum::List::__getitem__ %{
        val.__fill_distrib__()
%}


%pythonappend gum::BayesNetInference::marginal %{
        val.__fill_distrib__()
%}


%feature("shadow") gum::BayesNetInference<float>::setEvidence %{
def setEvidence(self, evidces):
    if not isinstance(evidces, dict):
        raise TypeError("setEvidence parameter must be dict, not %s"
                        %(type(evidces)))

    bn = self.bn()

    # set evidences
    list_pot = ListPotentials_double()
    for var_name, evidce in evidces.iteritems():
        pot = Potential_double()

        if isinstance(var_name, int):
            var = bn.variable(var_name)
        elif isinstance(var_name, str):
            var = bn.variableFromName(var_name)
        else:
            raise TypeError('values of the dict must be int or string')

        pot.add(var)
        if isinstance(evidce, (int, float, str)):
            pot[:] = 0
            # determine the var type
            try:
                cast_var = var.toLabelizedVar()
                if isinstance(evidce, int):
                    index = evidce
                elif isinstance(evidce, str):
                    index = cast_var[evidce]
                else:
                    raise TypeError('values of the dict must be int or string')
            except RuntimeError:
                try:
                    cast_var = var.toRangeVar()
                    if isinstance(evidce, int):
                        index = cast_var[str(evidce)]
                    elif isinstance(evidce, str):
                        index = cast_var[evidce]
                    else:
                        raise TypeError('values of the dict must be int or string')
                except RuntimeError:
                    cast_var = var.toDiscretizedVar()
                    if isinstance(evidce, float):
                        index = cast_var.index(evidce)
                    elif isinstance(evidce, str):
                        index = cast_var.index(float(evidce))
                    else:
                        raise TypeError('values of the dict must be float or string')
            pot[index] = 1
        elif isinstance(evidce, (list, tuple)):
            pot[:] = evidce
        else:
            raise TypeError('dict values must be number, string or sequence')
        list_pot.append(pot)

    self.insertEvidence(list_pot)
%}


// copy: M indicates the modifications
%feature("shadow") gum::BayesNetInference<double>::setEvidence %{
def setEvidence(self, evidces):
    if not isinstance(evidces, dict):
        raise TypeError("setEvidence parameter must be dict, not %s"
                        %(type(evidces)))
    bn = self.bn()

    # set evidences
    list_pot = ListPotentials_double() #M
    for var_name, evidce in evidces.iteritems():
        pot = Potential_double() #M

        if isinstance(var_name, int):
            var = bn.variable(var_name)
        elif isinstance(var_name, str):
            var = bn.variableFromName(var_name)
        else:
            raise TypeError('values of the dict must be int or string')

        pot.add(var)
        if isinstance(evidce, (int, float, str)):
            pot[:] = 0
            # determine the var type
            try:
                cast_var = var.toLabelizedVar()
                if isinstance(evidce, int):
                    index = evidce
                elif isinstance(evidce, str):
                    index = cast_var[evidce]
                else:
                    raise TypeError('values of the dict must be int or string')
            except RuntimeError:
                try:
                    cast_var = var.toRangeVar()
                    if isinstance(evidce, int):
                        index = cast_var[str(evidce)]
                    elif isinstance(evidce, str):
                        index = cast_var[evidce]
                    else:
                        raise TypeError('values of the dict must be int or string')
                except RuntimeError:
                    cast_var = var.toDiscretizedVar()
                    if isinstance(evidce, float):
                        index = cast_var.index(evidce)
                    elif isinstance(evidce, str):
                        index = cast_var.index(float(evidce))
                    else:
                        raise TypeError('values of the dict must be float or string')
            pot[index] = 1
        elif isinstance(evidce, (list, tuple)):
            pot[:] = evidce
        else:
            raise TypeError('dict values must be number, string or sequence')
        list_pot.append(pot)

    self.insertEvidence(list_pot)
%}


// these void class extensions are rewritten by "shadow" declarations
%extend gum::BayesNetInference<float> {
    void setEvidence(PyObject *evidces) {}
}
%extend gum::BayesNetInference<double> {
    void setEvidence(PyObject *evidces) {}
}


%feature("shadow") gum::LazyPropagation::__del__ %{
    def __del__(self):
        self.__disown__()
%}


%extend gum::LazyPropagation {
    void __del__() {}
}


%feature("shadow") gum::GibbsInference::__del__ %{
    def __del__(self):
        self.__disown__()
%}


%extend gum::GibbsInference {
    void __del__() {}
}


%extend gum::BruteForceKL {
PyObject* compute(void) {
  PyObject* q=PyDict_New();

  PyDict_SetItemString(q,"klPQ",PyFloat_FromDouble(self->klPQ()));
  PyDict_SetItemString(q,"errorPQ",PyInt_FromLong(self->errorPQ()));
  PyDict_SetItemString(q,"klQP",PyFloat_FromDouble(self->klQP()));
  PyDict_SetItemString(q,"errorQP",PyInt_FromLong(self->errorQP()));
  PyDict_SetItemString(q,"hellinger",PyFloat_FromDouble(self->hellinger()));
  PyDict_SetItemString(q,"bhattacharya",PyFloat_FromDouble(self->bhattacharya()));

  return q;
}
}

%extend gum::GibbsKL {
PyObject* compute(void) {
  PyObject* q=PyDict_New();

  PyDict_SetItemString(q,"klPQ",PyFloat_FromDouble(self->klPQ()));
  PyDict_SetItemString(q,"errorPQ",PyInt_FromLong(self->errorPQ()));
  PyDict_SetItemString(q,"klQP",PyFloat_FromDouble(self->klQP()));
  PyDict_SetItemString(q,"errorQP",PyInt_FromLong(self->errorQP()));
  PyDict_SetItemString(q,"hellinger",PyFloat_FromDouble(self->hellinger()));
  PyDict_SetItemString(q,"bhattacharya",PyFloat_FromDouble(self->bhattacharya()));

  return q;
}
}

%{
#include "extensions/PythonBNListener.h"
#include "extensions/PythonLoadListener.h"
/*#include "extensions/PythonGibbsKLListener.h"*/




    int __fillListeners(std::vector<PythonLoadListener>& py_listener, PyObject *l) {
        if (!l) return 0;
        if (l==Py_None) return 0;

        int l_size = 0;
        PyObject *item;

        if(PySequence_Check(l)) {
            l_size = PySequence_Size(l);
            py_listener.resize(l_size);
            for(int i=0 ; i < l_size ; i++) {
                item = PySequence_GetItem(l, i);
                if(! py_listener[i].setPythonListener(item)) return false;
            }
        } else {
            if(! py_listener[0].setPythonListener(l)) return false;
        }

        return l_size;
    };
%}

/* INCLUDE aGrUM base wrap */
%include "aGrUM_wrap.i"

%include "extensions/PythonBNListener.h"
%include "extensions/PythonLoadListener.h"
/*%include "extensions/PythonGibbsKLListener.h"*/



/* TEMPLATES INSTANTIATIONS */
/*
%template(PythonGibbsKLListener_float) PythonGibbsKLListener<float>;
%template(PythonGibbsKLListener_double) PythonGibbsKLListener<double>;

%extend gum::GibbsKL_double {
%pythoncode {
    def setListeners(self,whenProgress,whenStop):
        self.listener=PythonGibbsKLListener_double(self)
        self.listener.setWhenProgress(onProgress)
        self.listener.setWhenStop(onStop)
}
}

%extend gum::GibbsKL_float {
%pythoncode {
    def setListeners(self,whenProgress,whenStop):
        self.listener=PythonGibbsKLListener_float(self)
        self.listener.setWhenProgress(onProgress)
        self.listener.setWhenStop(onStop)
}
}
*/

%pythoncode %{
Potential = Potential_double
ListPotentials = ListPotentials_double
BayesNet = BayesNet_double
LazyPropagation = LazyPropagation_double
GibbsInference = GibbsInference_double
BruteForceKL = BruteForceKL_double
GibbsKL = GibbsKL_double
# PythonGibbsKLListener=PythonGibbsKLListener_double
%}


