import pyagrum as gum

g = gum.fastDiGraph("A->B")
g.nameFromId(1)
# 'B'
g.addNode()
# 2
g.nameFromId(2)  # no name: the id as a string
# '2'
