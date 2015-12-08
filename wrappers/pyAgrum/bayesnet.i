%ignore gum::BayesNet::addVariable;
%ignore gum::BayesNet::eraseVariable;
%ignore gum::BayesNet::insertArc;
%ignore gum::BayesNet::insertWeightedArc;
%ignore *::nbrArcs;

%ignore *::beginNodes;
%ignore *::endNodes;
%ignore *::beginNodesSafe;
%ignore *::endNodesSafe;
%ignore *::beginArcs;
%ignore *::endArcs;

%pythonappend gum::BayesNet::cpt %{
        val.__fill_distrib__()
%}

%typemap(out) const gum::Sequence<gum::NodeId>& {
  PyObject *q=PyList_New(0);
  for(auto i : *$1) {
    PyList_Append(q,PyInt_FromLong(i));
  }
  $result=q;
}

%ignore *::nodes;

%include "extensions/BNGenerator.h"

%{
#include "extensions/BNGenerator.h"
%}


%extend gum::BayesNet {
    PyObject *names() const {
      PyObject* q=PyList_New(0);

      const gum::DAG& dag=self->dag();
      for ( gum::NodeGraphPartIterator node_iter = dag.nodes().begin();node_iter != dag.nodes().end(); ++node_iter ) {
        PyList_Append(q,PyString_FromString(self->variable(*node_iter).name().c_str()));
      }
      return q;
    };

    PyObject *ids() {
      PyObject* q=PyList_New(0);

      const gum::DAG& dag=self->dag();
      for ( gum::NodeGraphPartIterator  node_iter = dag.nodes().begin();node_iter != dag.nodes().end(); ++node_iter ) {
        PyList_Append(q,PyInt_FromLong(*node_iter));
      }

      return q;
    };

    PyObject *nodes() {
      return ids();
    }

    PyObject *arcs() {
      PyObject* q=PyList_New(0);

      const gum::DAG& dag=self->dag();
      for ( auto arc_iter = dag.arcs().begin();arc_iter != dag.arcs().end(); ++arc_iter ) {
        PyList_Append(q,Py_BuildValue("(i,i)", arc_iter->tail(), arc_iter->head()));
      }

      return q;
    };


    PyObject *parents(const NodeId id) const {
    PyObject* q=PyList_New(0);

    const gum::NodeSet& p=self->dag().parents(id);
    for(gum::NodeSet::const_iterator it=p.begin();it!=p.end();++it) {
      PyList_Append(q,PyInt_FromLong(*it));
    }

    return q;
  };

  PyObject *children(const NodeId id) const {
    PyObject* q=PyList_New(0);

    const gum::NodeSet& p=self->dag().children(id);
    for(gum::NodeSet::const_iterator it=p.begin();it!=p.end();++it) {
      PyList_Append(q,PyInt_FromLong(*it));
    }

    return q;
  };

  std::string loadBIF(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::BIFReader<GUM_SCALAR> reader(self,name);
          int l_size=__fillLoadListeners(py_listener,l);
          for(int i=0 ; i<l_size ; i++) {
              GUM_CONNECT(reader.scanner(), onLoad, py_listener[i], PythonLoadListener::whenLoading);
          }

          auto nbErr=reader.proceed();
          reader.showElegantErrorsAndWarnings(stream);
          if (nbErr>0) {
              reader.showErrorCounts(stream);
              GUM_ERROR(gum::FatalError,stream.str());
          } else {
              return stream.str();
          }
      } catch (gum::IOError& e) {
        throw(e);
      }
      return "";
  };

  void saveBIF(std::string name) {
      gum::BIFWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };

  std::string loadDSL(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::DSLReader<GUM_SCALAR> reader(self,name);
          int l_size=__fillLoadListeners(py_listener,l);
          for(int i=0 ; i<l_size ; i++) {
              GUM_CONNECT(reader.scanner(), onLoad, py_listener[i], PythonLoadListener::whenLoading);
          }

          auto nbErr=reader.proceed();
          reader.showElegantErrorsAndWarnings(stream);
          if (nbErr>0) {
              reader.showErrorCounts(stream);
              GUM_ERROR(gum::FatalError,stream.str());
          } else {
              return stream.str();
          }
      } catch (gum::IOError& e) {
        throw(e);
      }
      return "";
  };

  void saveDSL(std::string name) {
      gum::DSLWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };

  std::string loadNET(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::NetReader<GUM_SCALAR> reader(self,name);
          int l_size=__fillLoadListeners(py_listener,l);
          for(int i=0 ; i<l_size ; i++) {
              GUM_CONNECT(reader.scanner(), onLoad, py_listener[i], PythonLoadListener::whenLoading);
          }

          auto nbErr=reader.proceed();
          reader.showElegantErrorsAndWarnings(stream);
          if (nbErr>0) {
              reader.showErrorCounts(stream);
              GUM_ERROR(gum::FatalError,stream.str());
          } else {
              return stream.str();
          }
      } catch (gum::IOError& e) {
        throw(e);
      }
      return "";
  };

  void saveNET(std::string name) {
      gum::NetWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };

  std::string loadPRM(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::O3prmBNReader<GUM_SCALAR> reader(self,name);

          auto nbErr=reader.proceed();
          reader.showElegantErrorsAndWarnings(stream);
          if (nbErr>0) {
              reader.showErrorCounts(stream);
              GUM_ERROR(gum::FatalError,stream.str());
          } else {
              return stream.str();
          }
      } catch (gum::IOError& e) {
        throw(e);
      }
      return "";
  };

  std::string loadBIFXML(std::string name, PyObject *l=(PyObject*)0)
  {
      std::stringstream stream;
      std::vector<PythonLoadListener> py_listener;

      try {
          gum::BIFXMLBNReader<GUM_SCALAR> reader(self,name);

          int isOK=reader.proceed();// for BIFXML, proceed() returns 0 or 1
          if (isOK==0) {
              GUM_ERROR(gum::FatalError,"Errors found");
          } else {
              return "";
          }
      } catch (gum::IOError& e) {
        throw(e);
      }
      return "";
  };

  void saveBIFXML(std::string name) {
      gum::BIFXMLBNWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };
}

