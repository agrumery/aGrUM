Input/Output Formats for Bayesian Networks
==========================================

PyAgrum supports several formats for saving and loading graphical models, but not all formats preserve every feature of the models. Below is a guide to the supported formats and their capabilities.

Supported Formats
-----------------


.. _bifxml-format:

BIFXML (Recommended for Complete Preservation)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The BIFXML format is the recommended way to save and load graphical models as it preserves all features and attributes of the model. This format is particularly useful for interoperability with other Bayesian network tools.

Example usage::

    import pyAgrum as gum
    
    # Save to BIFXML
    gum.saveBN(bn, "model.bifxml")
    
    # Load from BIFXML
    bn = gum.loadBN("model.bifxml")

.. _pickle-format:

Pickle (Alternative for Complete Preservation)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Python ``pickle`` format 

Example usage::

    import pyAgrum as gum
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

Other Supported Formats (With Limitations)
-----------------------------------------

The following formats are supported but may not preserve all model features:

- **Net** (`.net`): Standard format for Bayesian networks but may lose some metadata
- **DSL** (`.dsl`): GeNIe format with some limitations
- **UAI** (`.uai`): Format for Bayesian networks and Markov networks with limitations
- **BIF** (`.bif`): Format for Bayesian networks and Markov networks with limitations
- etc. (see gum.availableBNExts() for a complete list).
.. warning::
   Only **Pickle** and **BIFXML** formats guarantee complete preservation of all graphical model features, including:
   
   - All node attributes and metadata
   - Complex probability distributions
   - Learning parameters
   - All types of graphical models (BNs, MNs, IDs, etc.)
   
   Other formats may lose some of this information during serialization.