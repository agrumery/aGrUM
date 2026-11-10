import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.setNodeBgColor(a, "#FF7F64")  # the font becomes black on this light color
print(next(line.strip() for line in bn.toDot(options).splitlines() if line.strip().startswith('"A" [')))
# "A" [tooltip="(0) A", fillcolor="#FF7F64", fontcolor="black"];
