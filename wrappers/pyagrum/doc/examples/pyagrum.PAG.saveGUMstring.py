import pyagrum as gum

pag = gum.fastPAG("A o-> B;B <-o C")
print(pag.saveGUMstring(indent=-1))
# {"type":"PAG","GumJsonVersion":"1.0","nodes":[0,1,2],"names":{"0":"A","1":"B","2":"C"},"arcs":[],"edges":[],"markedEdges":[[0,1,"circle","arrowhead"],[1,2,"arrowhead","circle"]]}
