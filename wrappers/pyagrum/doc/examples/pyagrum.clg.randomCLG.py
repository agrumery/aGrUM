# requires: pandas, scipy, sklearn
import pyagrum as gum
import pyagrum.clg as gclg

gum.initRandom(42)
clg = gclg.randomCLG(5, ["A", "B", "C", "D", "E"])
sorted(clg.names())
# ['A', 'B', 'C', 'D', 'E']
len(clg.variables())
# 5
