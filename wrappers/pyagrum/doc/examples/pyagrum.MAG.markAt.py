import pyagrum as gum

dag = gum.fastDAG("A->X;Y<-B;X<-T->Y")
mag = gum.MAG.fromDAG(dag, {4})  # T latent: A->X<->Y<-B
mag.markAt(1, 0)  # the mark at A on A->X
# <EdgeMark.Tail: 1>
mag.markAt(1, 2)  # the mark at Y on X<->Y
# <EdgeMark.Arrowhead: 2>
