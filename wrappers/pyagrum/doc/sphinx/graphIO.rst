Reading and writing graphs
==========================

The graphs are saved in the native GUM format of aGrUM: ``.jgum`` (JSON text) or ``.bgum`` (binary), with their
class, the names of their nodes, their arcs, edges and marks. They are also pickled through this format, and
:func:`pyagrum.saveGraph` / :func:`pyagrum.loadGraph` also accept ``.pkl`` files (see
:func:`pyagrum.availableGraphExts`).

.. pyagrum-functions::

   pyagrum.saveGraph
   pyagrum.loadGraph
   pyagrum.availableGraphExts
   pyagrum.graphFromGUMString
