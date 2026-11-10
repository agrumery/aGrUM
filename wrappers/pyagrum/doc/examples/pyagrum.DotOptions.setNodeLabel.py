import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.setNodeLabel(b, "Bee")
print(next(line.strip() for line in bn.toDot(options).splitlines() if line.strip().startswith('"B" [')))
# "B" [label="Bee", tooltip="(1) B"];
