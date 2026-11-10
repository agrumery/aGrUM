import pyagrum as gum

pdag = gum.fastPDAG("A->B<-C;B-D")
pdag.parents(1)  # the edge B-D is not counted
# {0, 2}
