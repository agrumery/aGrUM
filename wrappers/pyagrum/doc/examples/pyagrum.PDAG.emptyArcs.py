import pyagrum as gum

pdag = gum.fastPDAG("A-B-C")
pdag.emptyArcs()
# True
pdag.addArc(0, 2)
pdag.emptyArcs()
# False
