import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.setRankDir("LR")  # left to right
print(next(line.strip() for line in bn.toDot(options).splitlines() if line.strip().startswith("graph [")))
# graph [bgcolor="transparent", size="5", rankdir="LR", layout="dot"];
