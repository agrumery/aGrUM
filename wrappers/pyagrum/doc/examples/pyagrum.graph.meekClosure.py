import pyagrum as gum
import pyagrum.graph as gg

gg.meekClosure(gum.fastPDAG("A->B-C")).arcs()  # rule R1
# {(0, 1), (1, 2)}
