# requires: IPython, matplotlib, pydot
import pydot as dot

import pyagrum as gum
import pyagrum.lib.notebook as gnb

graph = dot.graph_from_dot_data(gum.fastBN("A->B").toDot())[0]
gnb.showGraph(graph, size="2")
