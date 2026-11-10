import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.setNodeFgColor(a, "yellow")
print(next(line.strip() for line in bn.toDot(options).splitlines() if line.strip().startswith('"A" [')))
# "A" [tooltip="(0) A", fontcolor="yellow"];
