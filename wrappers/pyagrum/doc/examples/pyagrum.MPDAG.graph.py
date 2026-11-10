import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
pdag = mpdag.graph()  # a modifiable copy
pdag.eraseArc(2, 3)
type(pdag).__name__, mpdag.sizeArcs(), pdag.sizeArcs()
# ('PDAG', 2, 1)
