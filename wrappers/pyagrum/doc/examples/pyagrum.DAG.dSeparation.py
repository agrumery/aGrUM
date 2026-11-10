import pyagrum as gum

dag = gum.fastDAG("A->B<-C;B->D")
dag.dSeparation(0, 2)  # A and C are d-separated
# True
dag.dSeparation(0, 2, 3)  # but not given D, a descendant of the collider B
# False
dag.dSeparation([0], [3], [1])
# True
