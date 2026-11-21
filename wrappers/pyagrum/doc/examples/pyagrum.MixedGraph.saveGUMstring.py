import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
print(g.saveGUMstring(indent=-1))
# {"type":"MixedGraph","GumJsonVersion":"1.1","nodes":[0,1,2],"names":{"0":"A","1":"B","2":"C"},"arcs":[[0,1]],"edges":[[1,2]],"markedEdges":[]}
