import pyagrum as gum

jt = gum.JunctionTreeGenerator().junctionTree(gum.fastBN("A->B->C"))
try:
  jt.saveGUMstring()  # the cliques can not be stored in the GUM format
except NotImplementedError:
  print("not available for a CliqueGraph")
