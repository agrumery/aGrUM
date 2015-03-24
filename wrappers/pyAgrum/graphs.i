%ignore *::insertArc;
%ignore *::insertEdge;
%ignore *::insertNode;
/*
%ignore gum::CliqueGraph::addNode(const gum::NodeId,const gum::NodeSet&);
*/

%define ADD_IDS_METHOD_TO_GRAPHCLASS(classname)
%extend classname {
  PyObject *ids() {
    PyObject* q=PyList_New(0);

    for ( auto node : self->nodes()) {
      PyList_Append(q,PyInt_FromLong(node));
    }

    return q;
  };
};
%enddef
ADD_IDS_METHOD_TO_GRAPHCLASS(gum::DiGraph); // add for the sub-classes (including MixedGraph)
ADD_IDS_METHOD_TO_GRAPHCLASS(gum::UndiGraph);


%extend gum::DiGraph {
  PyObject *arcs() { // add for the sub-classes (including MixedGraph)
    PyObject* q=PyList_New(0);

    for ( auto arc : self->arcs()) {
      PyList_Append(q,Py_BuildValue("(i,i)", arc.tail(), arc.head()));
    }

    return q;
  };
};

%extend gum::UndiGraph {
  PyObject *edges() { // add for the sub-classes (including MixedGraph)
    PyObject* q=PyList_New(0);

    for ( auto edge :  self->edges()) {
      PyList_Append(q,Py_BuildValue("(i,i)", edge.first(), edge.second()));
    }

    return q;
  };

  PyObject *neighbours(gum::NodeId id) {
    PyObject* q=PySet_New(0);

    for ( auto node : self->neighbours(id)) {
      PySet_Add(q,PyInt_FromLong(node));
    }

    return q;
  };
};


%extend gum::CliqueGraph {
  PyObject *clique(const gum::NodeId clique) const {
    PyObject* q=PySet_New(0);
    
    for(auto node : self->clique(clique)) {
          PySet_Add(q,PyInt_FromLong(node));
        }
        
    return q;
  };
};
%ignore gum::CliqueGraph::clique;
%ignore gum::CliqueGraph::addNode;
%ignore gum::CliqueGraph::addEdge(const gum::NodeId,const gum::NodeId);