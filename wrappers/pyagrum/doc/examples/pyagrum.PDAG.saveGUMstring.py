import pyagrum as gum

pdag = gum.fastPDAG("A->B-C")
print(pdag.saveGUMstring(indent=-1))
# {"type":"PDAG","GumJsonVersion":"1.0","nodes":[0,1,2],"names":{"0":"A","1":"B","2":"C"},"arcs":[[0,1]],"edges":[[1,2]],"markedEdges":[]}
print(gum.graphFromGUMString(pdag.saveGUMstring()))
# {0<A>,1<B>,2<C>} , {0->1} , {1--2}
