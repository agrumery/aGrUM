import pyagrum as gum

bn = gum.fastBN("A;B;C;D")
bn.beginTopologyTransformation()  # the CPTs are resized once, at the end
for x in "ABC":
  bn.addArc(x, "D")
bn.endTopologyTransformation()
bn.cpt("D").names
# ('D', 'A', 'B', 'C')
