import pyagrum as gum

mrf = gum.fastMRF("A--B;C")
edges = []
listener = mrf.addStructureListener(whenNodeAdded=lambda i, name: None)
listener.setWhenEdgeAdded(lambda i, j: edges.append({i, j}))  # the order of i and j is not specified
mrf.addFactor(["B", "C"])
edges
# [{1, 2}]
