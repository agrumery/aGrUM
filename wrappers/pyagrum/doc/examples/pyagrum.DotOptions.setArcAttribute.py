import pyagrum as gum

bn = gum.fastBN("A->B")
a, b = bn.idFromName("A"), bn.idFromName("B")
options = gum.DotOptions()
options.setArcAttribute(a, b, "style", "dashed")
print(next(line.strip() for line in bn.toDot(options).splitlines() if '"A" -> "B"' in line))
# "A" -> "B" [fontsize="10", tooltip="0&nbsp;&rarr;&nbsp;1", style="dashed"];
