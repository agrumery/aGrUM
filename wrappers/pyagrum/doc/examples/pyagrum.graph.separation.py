import pyagrum as gum
import pyagrum.graph as gg

dag = gum.fastDAG("A->C<-B")
gg.separation(dag, "A", "B")
# True
gg.separation(dag, "A", "B", "C")
# False
chain = gum.fastChainGraph("A->B-C<-D")  # the parents of the chain component {B, C} are married
gg.separation(chain, "A", "D", "B")
# False
