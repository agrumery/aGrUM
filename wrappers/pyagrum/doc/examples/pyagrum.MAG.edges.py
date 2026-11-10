import pyagrum as gum

dag = gum.fastDAG("A->X;Y<-B;X<-T->Y")
mag = gum.MAG.fromDAG(dag, {4})  # T latent: A->X<->Y<-B
sorted(mag.edges())  # whatever the marks
# [(0, 1), (1, 2), (2, 3)]
