import pyagrum as gum

dag = gum.fastDAG("A->X;Y<-B;X<-T->Y")
mag = gum.MAG.fromDAG(dag, {4})  # T latent: A->X<->Y<-B
mag.saveGUM("mag.jgum")
print(gum.loadGraph("mag.jgum"))
# {0<A>,1<X>,2<Y>,3<B>} , {0 --> 1, 2 <-- 3, 1 <-> 2}
