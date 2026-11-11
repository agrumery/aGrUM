import pyagrum as gum

bn = gum.fastBN("A->B->C")
options = gum.DotOptions()
options.setSize("12!")
options.setNodeBgColor(bn.idFromName("A"), "#FF7F64")
options.setArcWidth(bn.idFromName("A"), bn.idFromName("B"), 3)
dot = bn.toDot(options)
print("\n".join(line.strip() for line in dot.splitlines() if '"A"' in line or line.strip().startswith("graph [")))
# graph [bgcolor="transparent", size="12!", rankdir="TB", layout="dot"];
# "A" [tooltip="(0) A", fillcolor="#FF7F64", fontcolor="black"];
# "A" -> "B" [fontsize="10", tooltip="0&nbsp;&rarr;&nbsp;1", penwidth="3"];
