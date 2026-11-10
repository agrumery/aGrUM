import pyagrum as gum
import pyagrum.graph as gg

gg.orientAsDAG(gum.fastPDAG("A->B-C"), {0: 2, 1: 2, 2: 2}).arcs()
# {(0, 1), (1, 2)}
