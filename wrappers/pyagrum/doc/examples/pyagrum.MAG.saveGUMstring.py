import pyagrum as gum

dag = gum.fastDAG("A->X;Y<-B;X<-T->Y")
mag = gum.MAG.fromDAG(dag, {4})  # T latent: A->X<->Y<-B
print(mag.saveGUMstring(indent=-1))
# {"type":"MAG","GumJsonVersion":"1.0","nodes":[0,1,2,3],"names":{"0":"A","1":"X","2":"Y","3":"B"},"arcs":[],"edges":[],"markedEdges":[[0,1,"tail","arrowhead"],[2,3,"arrowhead","tail"],[1,2,"arrowhead","arrowhead"]]}
