import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.setArcWidth(a, b, 3)
print(next(line.strip() for line in bn.toDot(options).splitlines() if '"A" -> "B"' in line))
# "A" -> "B" [fontsize="10", tooltip="0&nbsp;&rarr;&nbsp;1", penwidth="3"];
