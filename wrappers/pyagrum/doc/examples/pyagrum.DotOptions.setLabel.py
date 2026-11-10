import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.setLabel("my BN")
print(next(line.strip() for line in bn.toDot(options).splitlines() if line.strip().startswith("graph [")))
# graph [bgcolor="transparent", size="5", rankdir="TB", layout="dot", label="my BN", fontcolor="#4A4A4A"];
