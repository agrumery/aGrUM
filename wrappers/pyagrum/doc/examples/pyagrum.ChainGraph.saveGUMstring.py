import pyagrum as gum

cg = gum.fastChainGraph("A->B-C")
print(cg.saveGUMstring(indent=-1))
# {"type":"ChainGraph","GumJsonVersion":"1.0","nodes":[0,1,2],"names":{"0":"A","1":"B","2":"C"},"arcs":[[0,1]],"edges":[[1,2]],"markedEdges":[]}
