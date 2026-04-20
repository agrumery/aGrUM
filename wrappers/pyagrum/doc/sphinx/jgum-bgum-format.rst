.. _jgum-bgum-format:

JGUM / BGUM Format Reference
==============================

The **jgum** (JSON) and **bgum** (binary) formats are the native serialization formats for pyAgrum graphical models. They are the recommended formats for saving and loading models when interoperability with other tools is not required.

- **jgum** is a human-readable JSON text format, easy to inspect and debug.
- **bgum** is the binary equivalent encoded as `MessagePack <https://msgpack.org/>`_, which is more compact and faster to read/write. Both formats carry exactly the same information.

All three model types — :class:`~pyagrum.BayesNet`, :class:`~pyagrum.InfluenceDiagram`, and :class:`~pyagrum.MarkovRandomField` — are supported. The format is identified by a ``"type"`` field in the JSON root (``"BN"``, ``"ID"``, or ``"MRF"``), so attempting to load a file with a mismatched type raises an error.

.. note::
   The ``"properties"`` section carries metadata automatically managed by pyAgrum:
   ``software`` (library version), ``creation`` (ISO timestamp of first write),
   and ``lastModification`` (ISO timestamp of last write).
   User-defined properties set via :meth:`~pyagrum.BayesNet.setProperty` are also persisted.

.. seealso::
   :ref:`json-format` in the BN I/O guide for a usage-oriented overview.


Bayesian Network (``"BN"``)
----------------------------

Structure
~~~~~~~~~

.. code-block:: json

   {
     "type": "BN",
     "GumJsonVersion": "1.0",
     "nodes": [
       "A{Yes|Maybe|No}",
       "B[1,5,10,100]",
       "C[2]"
     ],
     "parents": {
       "A": [],
       "B": ["A"],
       "C": ["A", "B"]
     },
     "cpt": {
       "A": [0.3, 0.5, 0.2],
       "B": [0.8, 0.1, 0.05, 0.05, 0.6, 0.2, 0.1, 0.1, 0.4, 0.3, 0.2, 0.1],
       "C": [0.9, 0.1, 0.3, 0.7, 0.6, 0.4, 0.2, 0.8, 0.7, 0.3, 0.1, 0.9, 0.5, 0.5, 0.4, 0.6, 0.8, 0.2, 0.3, 0.7, 0.6, 0.4, 0.5, 0.5]
     },
     "properties": {
       "software": "aGrUM 1.2.3",
       "creation": "2026-04-20T10:00:00",
       "lastModification": "2026-04-20T10:00:00"
     }
   }

Fields
~~~~~~

``nodes``
  Array of variable descriptions in :ref:`fast syntax<Quick specification of (randomly parameterized) graphical models>`.
  Always present, even for an empty network.

``parents``
  Object mapping each variable name to an ordered list of its parent names.
  The order matches the CPT axis order (axis 0 is the variable itself, axes 1… are its parents).
  Always present, even for variables with no parents (empty array).

``cpt``
  Object mapping each variable name to a flat array of conditional probability values.
  Values are listed in the order produced by iterating over the joint instantiation
  of the variable and its parents (same order as :meth:`~pyagrum.Tensor.fillWith`).
  Always present, even for an empty network (empty object).

``properties`` *(optional)*
  Key/value pairs of string metadata. Absent if no properties have been set.

Python usage
~~~~~~~~~~~~

.. code-block:: python

   import pyagrum as gum

   bn = gum.fastBN("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A")

   # save / load via file
   gum.saveBN(bn, "model.jgum")          # JSON text
   gum.saveBN(bn, "model.bgum")          # MessagePack binary
   bn2 = gum.loadBN("model.jgum")

   # save / load as a string (useful for in-memory serialization)
   s = bn.saveGUMstring()
   bn3 = gum.BayesNet()
   bn3.loadGUMstring(s)

   # pickle uses jgum internally
   import pickle
   data = pickle.dumps(bn)
   bn4 = pickle.loads(data)


Influence Diagram (``"ID"``)
-----------------------------

Structure
~~~~~~~~~

.. code-block:: json

   {
     "type": "ID",
     "GumJsonVersion": "1.0",
     "chance":   ["A[2]", "C[2]"],
     "utility":  ["U[1]"],
     "decision": ["D[2]"],
     "parents": {
       "A": [],
       "C": ["A", "D"],
       "U": ["C"],
       "D": ["A"]
     },
     "cpt": {
       "A": [0.4, 0.6],
       "C": [0.9, 0.1, 0.3, 0.7, 0.8, 0.2, 0.5, 0.5]
     },
     "reward": {
       "U": [10.0, -5.0, 3.0, 8.0]
     },
     "properties": {}
   }

Fields
~~~~~~

``chance`` / ``utility`` / ``decision``
  Three arrays of variable descriptions (fast syntax), one per node type.
  All three sections are always present, even if empty.

``parents``
  Same structure as for BN.  For chance nodes the order matches the CPT axis order;
  for decision nodes it lists informational predecessors from the DAG.

``cpt``
  Conditional probability tables for **chance** nodes only (same encoding as BN).

``reward``
  Utility tables for **utility** nodes: flat arrays of numeric values iterated
  over the joint instantiation of the utility variable and its parents.

``properties`` *(optional)*
  Key/value pairs of string metadata.

Python usage
~~~~~~~~~~~~

.. code-block:: python

   import pyagrum as gum

   id_ = gum.fastID("A->C->*D->$U;A->$U")

   gum.saveID(id_, "model.jgum")
   id2 = gum.loadID("model.jgum")

   s = id_.saveGUMstring()
   id3 = gum.InfluenceDiagram()
   id3.loadGUMstring(s)


Markov Random Field (``"MRF"``)
--------------------------------

Structure
~~~~~~~~~

.. code-block:: json

   {
     "type": "MRF",
     "GumJsonVersion": "1.0",
     "nodes": ["A[2]", "B[2]", "C[2]"],
     "factors": [
       {"vars": ["A", "B"], "values": [0.5, 0.2, 0.3, 0.8]},
       {"vars": ["B", "C"], "values": [0.9, 0.1, 0.4, 0.6]}
     ],
     "properties": {}
   }

Fields
~~~~~~

``nodes``
  Array of variable descriptions in fast syntax.
  Always present, even for an empty MRF.

``factors``
  Array of factor objects, each with:

  - ``vars``: ordered list of variable names involved in the factor.
  - ``values``: flat array of factor values iterated over the joint instantiation
    of those variables (same order as :meth:`~pyagrum.Tensor.fillWith`).

  Always present, even when there are no factors (empty array).
  Unary factors (a single variable in ``vars``) are valid and encode node potentials.

``properties`` *(optional)*
  Key/value pairs of string metadata.

Python usage
~~~~~~~~~~~~

.. code-block:: python

   import pyagrum as gum

   mrf = gum.fastMRF("A--B--C;D--A")

   gum.saveMRF(mrf, "model.jgum")
   mrf2 = gum.loadMRF("model.jgum")

   s = mrf.saveGUMstring()
   mrf3 = gum.MarkovRandomField()
   mrf3.loadGUMstring(s)


Binary format (BGUM)
--------------------

The bgum format encodes the same JSON object using
`MessagePack <https://msgpack.org/>`_ (via ``json::to_msgpack`` from the
`nlohmann/json <https://json.nlohmann.me/>`_ library).  The file layout is:

- 8 bytes: little-endian ``uint64_t`` giving the byte length *N* of the payload.
- *N* bytes: the raw MessagePack payload.

The bgum format is not human-readable but is more compact and faster to
parse for large models.  The ``"type"`` check and all semantic rules are
identical to jgum.

.. code-block:: python

   import pyagrum as gum

   bn = gum.fastBN("A->B->C")
   gum.saveBN(bn, "model.bgum")   # binary write
   bn2 = gum.loadBN("model.bgum") # binary read
   assert bn == bn2
