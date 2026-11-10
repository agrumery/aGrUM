import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C;C--D")
ie = gum.ShaferShenoyMRFInference(mrf)
ie.makeInference()
jt = ie.junctionTree()
sorted(sorted(jt.clique(c)) for c in jt.nodes())
# [[0, 1], [1, 2], [2, 3]]
jt.sizeEdges()
# 2
