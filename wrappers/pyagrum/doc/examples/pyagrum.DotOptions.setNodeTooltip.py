import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.setNodeTooltip(a, "the cause")
print(next(line.strip() for line in bn.toDot(options).splitlines() if line.strip().startswith('"A" [')))
# "A" [tooltip="the cause"];
