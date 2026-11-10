import pyagrum as gum

g = gum.fastUndiGraph("A-B")
print(g.saveGUMstring(indent=-1))
# {"type":"UndiGraph","GumJsonVersion":"1.0","nodes":[0,1],"names":{"0":"A","1":"B"},"arcs":[],"edges":[[0,1]],"markedEdges":[]}
