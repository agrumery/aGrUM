import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
factor = {mrf.idFromName("A"), mrf.idFromName("B")}
options = gum.DotOptions()
options.setFactorBgColor(factor, "red")
print(
  next(line.strip() for line in mrf.toDotAsFactorGraph(options).splitlines() if line.strip().startswith('"f#A#B" ['))
)
# "f#A#B" [shape="point", width="0.1", height="0.1", fillcolor="red", fontcolor="white"];
