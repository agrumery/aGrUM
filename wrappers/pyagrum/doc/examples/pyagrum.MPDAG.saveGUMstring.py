import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
print(mpdag.saveGUMstring(indent=-1))
# {"type":"MPDAG","GumJsonVersion":"1.1","nodes":[0,1,2,3],"names":{"0":"A","1":"B","2":"C","3":"D"},"arcs":[[2,3],[1,2]],"edges":[[0,1]],"markedEdges":[]}
