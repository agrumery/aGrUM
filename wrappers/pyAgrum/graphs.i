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

    for ( auto node_iter = self->nodes().begin();
          node_iter != self->end(); 
          ++node_iter ) {
      PyList_Append(q,PyInt_FromLong(*node_iter));
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

    for ( auto arc_iter = self->arcs().begin();
          arc_iter != self->arcs().end(); 
          ++arc_iter ) {
      PyList_Append(q,Py_BuildValue("(i,i)", arc_iter->tail(), arc_iter->head()));
    }

    return q;
  };
};

%extend gum::UndiGraph {
  PyObject *edges() { // add for the sub-classes (including MixedGraph)
    PyObject* q=PyList_New(0);

    for ( auto edge_iter = self->edges().begin();
          edge_iter != self->edges().end(); 
          ++edge_iter ) {
      PyList_Append(q,Py_BuildValue("(i,i)", edge_iter->first(), edge_iter->second()));
    }

    return q;
  };
};


%extend gum::CliqueGraph {
  PyObject *clique(const gum::NodeId clique) const {
    PyObject* q=PySet_New(0);
    
    for(auto n_iter = self->clique(clique).begin();
        n_iter != self->clique(clique).end();
        ++n_iter) {
          PySet_Add(q,PyInt_FromLong(*n_iter));
        }
        
    return q;
  };
}
%ignore gum::CliqueGraph::clique;