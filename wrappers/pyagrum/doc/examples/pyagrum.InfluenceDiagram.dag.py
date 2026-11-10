import pyagrum as gum

diag = gum.fastID("C->*D->$U<-C")
d = diag.dag()
print(d)
# {0<C>,1<D>,2<U>} , {0->1,1->2,0->2}
d.parents(2)
# {0, 1}
