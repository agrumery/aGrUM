import pyagrum as gum

g = gum.fastDiGraph("A->B<-C;D->E")
sorted(g.connectedComponentsList().values(), key=min)
# [{0, 1, 2}, {3, 4}]
