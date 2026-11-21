import pyagrum as gum

eg = gum.EssentialGraph(gum.fastBN("A->B<-C"))
print(eg.saveGUMstring(indent=-1))
# {"type":"EssentialGraph","GumJsonVersion":"1.1","nodes":[0,1,2],"names":{"0":"A","1":"B","2":"C"},"arcs":[[0,1],[2,1]],"edges":[],"markedEdges":[]}
