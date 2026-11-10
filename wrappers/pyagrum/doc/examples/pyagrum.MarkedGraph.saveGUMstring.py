import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
print(g.saveGUMstring(indent=-1))
# {"type":"MarkedGraph","GumJsonVersion":"1.0","nodes":[0,1,2],"names":{"0":"A","1":"B","2":"C"},"arcs":[],"edges":[],"markedEdges":[[0,1,"tail","arrowhead"],[1,2,"arrowhead","arrowhead"]]}
print(gum.graphFromGUMString(g.saveGUMstring()))
# {0<A>,1<B>,2<C>} , {0 --> 1, 1 <-> 2}
