import pyAgrum.lib.notebook as gnb
import pydotplus as dot

def showGraphComparison(bn1, bn2, show = True):
    """ Trace a graph that compares the arcs of bn1 (reference) with those of bn2.
    full black line: the arc is common for both
    full red line: the arc is common but inverted in bn2
    dotted black line: the arc is added in bn2
    dotted red line: the arc is removed in bn2

    :param BayesNet_double bn1: referent model for the comparison
    :param BayesNet_double bn2: bn compared to the referent model
    :param boolean show: show the result graph in a notebook
    :return: the result dot graph
    """

    if set([i for i in bn1.names()]) == set([i for i in bn2.names()]):

        graph = dot.Dot(graph_type='digraph',bgcolor="transparent")

        # Nodes
        for n in bn1.names():
            node = dot.Node('"' + n + '"', style="filled",
                            bgcol = "#444444",
                            fgcol = "#FFFFFF",
                            tooltip='"({0}) {1}{2}"'.format(bn1.idFromName(n), n, ""))
            graph.add_node(node)

        # Arcs
        for n1 in bn2.names():
            for n2 in bn2.names():
                id1 = bn2.idFromName(n1)
                id2 = bn2.idFromName(n2)
                ID1 = bn1.idFromName(n1)
                ID2 = bn1.idFromName(n2)

                if bn1.dag().existsArc(ID1, ID2) and bn2.dag().existsArc(id1, id2):
                    # If present in both
                    edge = dot.Edge('"' + n1 + '"', '"' + n2 + '"')
                    graph.add_edge(edge)
                elif bn1.dag().existsArc(ID1, ID2) and bn2.dag().existsArc(id2, id1):
                    # If inversed in bn2
                    edge = dot.Edge('"' + n2 + '"', '"' + n1 + '"')
                    edge.set_color("red")
                    graph.add_edge(edge)
                elif bn1.dag().existsArc(ID1, ID2):
                    # If removed from bn2
                    edge = dot.Edge('"' + n1 + '"', '"' + n2 + '"', style='dashed')
                    edge.set_color("red")
                    graph.add_edge(edge)
                elif not bn1.dag().existsArc(ID1, ID2) and not bn1.dag().existsArc(ID2, ID1)  and bn2.dag().existsArc(id1, id2):
                    # If added to bn2
                    edge = dot.Edge('"' + n1 + '"', '"' + n2 + '"', style='dashed')
                    graph.add_edge(edge)

        if show:
            gnb.showGraph(graph)

    else:
        raise ValueError('Not the same variables in the BNs')

    return(graph)