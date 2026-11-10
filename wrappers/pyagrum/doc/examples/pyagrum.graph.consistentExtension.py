import pyagrum as gum
import pyagrum.graph as gg

gg.consistentExtension(gum.fastPDAG("A->B-C")).arcs()
# {(0, 1), (1, 2)}
print(gg.consistentExtension(gum.fastPDAG("A->B-C;D->C")))  # a new v-structure is unavoidable
# None
