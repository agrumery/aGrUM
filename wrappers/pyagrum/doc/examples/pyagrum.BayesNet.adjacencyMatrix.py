import pyagrum as gum

bn = gum.fastBN("A->B->C;A->C")
print(bn.adjacencyMatrix())
# [[0 1 1]
#  [0 0 1]
#  [0 0 0]]
