import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.saveUAI("model.uai")
mrf2 = gum.MarkovRandomField()
mrf2.loadUAI("model.uai")  # the warnings of the parser
# ''
mrf2.size(), mrf2.sizeEdges()
# (3, 2)
