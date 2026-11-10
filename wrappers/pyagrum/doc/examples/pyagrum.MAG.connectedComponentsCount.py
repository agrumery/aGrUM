import pyagrum as gum

mag = gum.MAG.fromDAG(gum.fastDAG("A->B;C->D;E"))
mag.connectedComponentsCount()
# 3
