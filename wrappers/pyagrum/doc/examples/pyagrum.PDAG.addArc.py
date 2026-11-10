import pyagrum as gum

pdag = gum.PDAG()
pdag.addNodes(3)
pdag.addArc(0, 1)
pdag.addEdge(1, 2)
print(pdag)
# {0,1,2} , {0->1} , {1--2}
