import pyagrum as gum

g = gum.fastDiGraph("A->B<-C;D->E")
cc = g.connectedComponents()  # node id -> component root
cc[0] == cc[1] == cc[2], cc[3] == cc[4], cc[0] == cc[3]
# (True, True, False)
