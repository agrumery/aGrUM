import pyagrum as gum

bn = gum.fastBN("A;B;C")
bn.beginTopologyTransformation()
bn.addArc("A", "C")
bn.addArc("B", "C")
bn.endTopologyTransformation()  # the CPT of C is resized here
bn.cpt("C").names
# ('C', 'A', 'B')
