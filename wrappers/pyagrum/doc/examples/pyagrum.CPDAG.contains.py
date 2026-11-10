import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.contains(gum.fastDAG("A;B;C;D;B->A;B->C;D->C"))  # B->A: same skeleton and v-structure
# True
cpdag.contains(gum.fastDAG("A->B<-C<-D"))  # a different v-structure
# False
