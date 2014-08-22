%ignore gum::InfluenceDiagram::variableNodeMap;
%ignore gum::InfluenceDiagram::insertArc;
%ignore gum::InfluenceDiagram::getPartialTemporalOrder;
%ignore gum::InfluenceDiagram:: addUtilityNode ( const gum::DiscreteVariable& variable,gum::MultiDimImplementation<GUM_SCALAR>* aContent, NodeId id = 0 );


%pythonappend gum::InfluenceDiagram::cpt %{
        val.__fill_distrib__()
%}

%pythonappend gum::InfluenceDiagram::utility %{
        val.__fill_distrib__()
%}

%typemap(out) const gum::Sequence<gum::NodeId>& {
  PyObject *q=PyList_New(0);
  for(auto i : *$1) {
    PyList_Append(q,PyInt_FromLong(i));
  }
  $result=q;
}

%include "extensions/IDGenerator.h"

%{
#include "extensions/IDGenerator.h"
%}


%extend gum::InfluenceDiagram {
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

   bool loadBIFXML(std::string name, PyObject *l=(PyObject*)0) {
    try {
        gum::BIFXMLIDReader<GUM_SCALAR> reader(self,name);
        reader.proceed(); 
        return true;
    } catch (gum::IOError& e) {
        throw(e);
    }

    return false;
  };

  void saveBIFXML(std::string name) {
      gum::BIFXMLIDWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };
}

