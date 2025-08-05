Input/Output Formats for Bayesian Networks
==========================================

PyAgrum supports several formats for saving and loading graphical models, but not all formats preserve every feature of the models. Below is a guide to the supported formats and their capabilities.

Supported Formats
-----------------

.. warning::
   Only json formats (**jgum** and **bgum**), **Pickle** and **BIFXML** formats guarantee complete preservation of all graphical model features.
   Other formats may lose some of this information during serialization.

.. _json-format:

JGUM/BGUM format (Recommended)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

JGUM and BGUM formats are JSON-based formats that ensure complete preservation of all features and attributes of the graphical model. They are the recommended formats for saving and loading Bayesian networks in PyAgrum.

  - BGUM is a binary version of JGUM, which is more compact and faster to read/write (`MessagePack <https://msgpack.org/>`_ format).
  - JGUM is a text-based format that is human-readable and easy to debug.

.. code-block:: python
    import pyagrum as gum

    # Save to JGUM/BGUM
    gum.config["core","default_jgumIndent"]=2  # Optional: set indentation for better readability (-1 = max compacity)
    gum.saveBN(bn, "model.jgum")
    gum.saveBN(bn, "binary_model.bgum")

    # Load from JGUM/BGUM
    bn = gum.loadBN("model.jgum")
    bn2 = gum.loadBN("model.bgum") # bn==bn2

.. _bifxml-format:

BIFXML (complete preservation)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The BIFXML format is the recommended way to save and load graphical models as it preserves all features and attributes of the model. This format is particularly useful for interoperability with other Bayesian network tools.

.. code-block:: python
    import pyagrum as gum

    # Save to BIFXML
    gum.saveBN(bn, "model.bifxml")

    # Load from BIFXML
    bn = gum.loadBN("model.bifxml")

.. _pickle-format:

Pickle (Alternative for complete preservation)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Python ``pickle`` format

.. code-block:: python
    import pyagrum as gum
    import pickle

    # Save model
    with open('model.pkl', 'wb') as f:
        pickle.dump(bn, f)

    # Load model
    with open('model.pkl', 'rb') as f:
        bn2 = pickle.load(f)

    # or easily
    gum.saveBN(bn, "model.pkl")
    bn2 = gum.loadBN("model.pkl")

Other Supported Formats
-----------------------

The following formats are supported but may not preserve all model features:

- **Net** (`.net`): Standard format for Bayesian networks but may lose some metadata
- **DSL** (`.dsl`): GeNIe format with some limitations
- **UAI** (`.uai`): Very Simple Format for Bayesian networks and Markov networks with limitations
- **BIF** (`.bif`): Format for Bayesian networks with limitations
- etc. (see :func:`pyagrum.availableBNExts()` for a complete list).
