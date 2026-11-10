import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
print(cpdag.saveGUMstring(indent=-1))
# {"type":"CPDAG","GumJsonVersion":"1.0","nodes":[0,1,2,3],"names":{"0":"A","1":"B","2":"C","3":"D"},"arcs":[[1,2],[3,2]],"edges":[[0,1]],"markedEdges":[]}
