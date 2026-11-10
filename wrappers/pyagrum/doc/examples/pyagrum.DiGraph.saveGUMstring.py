import pyagrum as gum

g = gum.fastDiGraph("A->B")
print(g.saveGUMstring(indent=-1))
# {"type":"DiGraph","GumJsonVersion":"1.0","nodes":[0,1],"names":{"0":"A","1":"B"},"arcs":[[0,1]],"edges":[],"markedEdges":[]}
sorted(gum.graphFromGUMString(g.saveGUMstring()).arcs())
# [(0, 1)]
