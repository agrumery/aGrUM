import pyagrum as gum

mag = gum.MAG.fromDAG(gum.fastDAG("A->B;C->D"))
cc = mag.connectedComponents()
cc[0] == cc[1], cc[0] == cc[2]
# (True, False)
