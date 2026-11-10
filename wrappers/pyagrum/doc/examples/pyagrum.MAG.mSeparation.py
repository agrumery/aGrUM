import pyagrum as gum

dag = gum.fastDAG("A->X;Y<-B;X<-T->Y")
mag = gum.MAG.fromDAG(dag, {4})  # T latent: A->X<->Y<-B
mag.mSeparation(0, 3)
# True
mag.mSeparation(0, 3, {1, 2})  # X and Y are colliders
# False
