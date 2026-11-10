import pyagrum as gum

dag = gum.fastDAG("A->X<-T->Y<-B")
mag = gum.MAG.fromDAG(dag, {dag.idFromName("T")})  # T latent
print(mag)
# {0<A>,1<X>,3<Y>,4<B>} , {0 --> 1, 1 <-> 3, 3 <-- 4}
dag = gum.fastDAG("A->S<-B;B->C")
print(gum.MAG.fromDAG(dag, set(), {dag.idFromName("S")}))  # S selected: A--B
# {0<A>,2<B>,3<C>} , {2 --> 3, 0 --- 2}
