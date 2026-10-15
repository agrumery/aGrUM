Drawing options for toDot
=========================

The look of the graphs drawn by ``toDot()`` (graphs, Bayesian networks, Markov random fields, influence diagrams, causal models, credal networks, junction trees...) is given by the keys of ``pyagrum.config`` (sections ``notebook``, ``factorgraph``, ``dynamicBN``, ``influenceDiagram``, ``credalnet`` and ``causal``, see :doc:`PyAgrumConfiguration`). A :class:`pyagrum.DotOptions` gives, for one drawing, what the configuration cannot: the size or the label of the graph, the color of some nodes, the width or the color of some arcs...

.. code-block:: python

    import pyagrum as gum
    bn = gum.fastBN("A->B->C")
    options = gum.DotOptions()
    options.setNodeBgColor(bn.idFromName("A"), "#FF7F64")
    options.setArcWidth(bn.idFromName("A"), bn.idFromName("B"), 3)
    dot = bn.toDot(options)

The font color of a node is ``adaptive`` by default (black or white according to the brightness of its background color), see the keys ``nodeFgColor`` of the configuration.

.. autoclass:: pyagrum.DotOptions
   :members:
