# requires: IPython, matplotlib, pydot
import pyagrum.lib.notebook as gnb

html = gnb.getDot("digraph { A -> B; }")
html.startswith("<svg")
# True
