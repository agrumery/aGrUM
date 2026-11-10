import pyagrum as gum

dag = gum.fastDAG("A->X;Y<-B;X<-T->Y")
mag = gum.MAG.fromDAG(dag, {4})  # T latent: A->X<->Y<-B
mag.adjacencyMatrix()
# array([[0, 1, 0, 0],
#        [1, 0, 1, 0],
#        [0, 1, 0, 1],
#        [0, 0, 1, 0]])
