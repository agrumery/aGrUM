Configuration for pyAgrum
=========================

Configuration for pyAgrum is centralized in an object ``pyagrum.config``, singleton of the (hidden) class ``PyAgrumConfiguration``.

Each key is declared in the defaults with a metadata line ``### type :: documentation`` where the type is ``int``, ``float``, ``bool``, ``str``, an enumeration of the allowed values (``svg|png``) or a deprecation (``deprecated -> section.key``). The setter refuses any value that does not respect the declared type and canonicalizes boolean values (any accepted spelling such as ``yes``/``on``/``1`` is stored as ``True`` or ``False``) as well as enumerated values (stored with their declared casing). ``config.typed[section, key]`` gives the value converted according to the declared type, and accessing a deprecated key emits a ``DeprecationWarning`` while transparently using its replacement.

See also the tutorial :doc:`notebooks/99-Tools_configForPyAgrum`.

.. autoclass:: pyagrum.config.PyAgrumConfiguration

Configuration reference
-----------------------

.. include:: generated/config-reference.inc
