# requires: matplotlib
import pyagrum as gum
import pyagrum.explain as expl

bn = gum.fastBN("A->B->Y;C->Y;Y->D")
html = expl.getInformation(bn, evs={"A": 1})
"<svg" in html
# True
