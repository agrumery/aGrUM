# requires: IPython, matplotlib, pydot, graphviz
import pyagrum.lib.notebook as gnb

html = gnb.getDot("digraph { A -> B; }")
html.startswith("<svg")
# True
