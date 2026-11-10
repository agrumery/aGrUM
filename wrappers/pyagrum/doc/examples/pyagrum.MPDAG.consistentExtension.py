import pyagrum as gum

mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A-B->C-D"))
dag = mpdag.consistentExtension()
mpdag.contains(dag), dag.sizeArcs()
# (True, 3)
