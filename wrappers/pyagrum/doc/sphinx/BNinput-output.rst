Input/Output Formats for Bayesian Networks
==========================================

PyAgrum supports several formats for saving and loading graphical models, but not all formats preserve every feature of the models. Below is a guide to the supported formats and their capabilities.

Supported Formats
-----------------

.. warning::
   Only **Pickle** and **BIFXML** formats guarantee complete preservation of all graphical model features
   Other formats may lose some of this information during serialization.

.. _bifxml-format:

BIFXML (Recommended for complete preservation)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
