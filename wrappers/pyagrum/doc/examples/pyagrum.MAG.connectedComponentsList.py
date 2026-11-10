import pyagrum as gum

mag = gum.MAG.fromDAG(gum.fastDAG("A->B;C->D"))
sorted(sorted(c) for c in mag.connectedComponentsList().values())
# [[0, 1], [2, 3]]
