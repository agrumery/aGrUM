import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.setSize("3!")  # exactly 3 inches
print(next(line.strip() for line in bn.toDot(options).splitlines() if line.strip().startswith("graph [")))
# graph [bgcolor="transparent", size="3!", rankdir="TB", layout="dot"];
