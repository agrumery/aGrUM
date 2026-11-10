import pyagrum as gum

g = gum.fastDiGraph("A->B<-C;D->E;F")
g.connectedComponentsCount()
# 3
