import pyagrum as gum

dag = gum.fastDAG("A->X<-T->Y<-B")
mag = gum.MAG.fromDAG(dag, {dag.idFromName("T")})  # T latent: A->X<->Y<-B
print(mag)
# {0<A>,1<X>,3<Y>,4<B>} , {0 --> 1, 1 <-> 3, 3 <-- 4}
