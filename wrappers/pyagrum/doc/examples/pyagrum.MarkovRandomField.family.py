import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
# no parent in an undirected model: the family of a node is the node itself
mrf.family("B")
# {1}
