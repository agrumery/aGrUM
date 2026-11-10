import pyagrum as gum

dag = gum.fastDAG("A->X;Y<-B;X<-T->Y")
mag = gum.MAG.fromDAG(dag, {4})  # T latent: not a node of the MAG
mag.existsNode(3), mag.existsNode(4)
# (True, False)
