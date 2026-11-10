import pyagrum as gum

mag = gum.MAG.fromDAG(gum.fastDAG("A->B"))
mag.empty()
# False
