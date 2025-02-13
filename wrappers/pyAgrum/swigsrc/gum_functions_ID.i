%pythoncode %{
import os.path as ospath

def availableIDExts():
  """ Give the list of all formats known by pyAgrum to save a influence diagram.

  Returns
  ------
  str
    a string which lists all suffixes for supported ID file formats.
  """
  return "xmlbif|bifxml|xml|pkl"


def loadID(filename):
  """
  read a gum.InfluenceDiagram from a ID file

  Parameters
  ----------
  filename: str
    the name of the input file

  Returns
  -------
  pyAgrum.InfluenceDiagram
    the InfluenceDiagram
  """
  extension = filename.split('.')[-1].upper()

  if extension in {"BIFXML", "XMLBIF", "XML"}:
    diag = pyAgrum.ID.InfluenceDiagram()
    # for now, just one format
    res = diag.loadBIFXML(filename)

    if not res:
      raise IOError(f"Error(s) in {filename}")
  elif extension == "PKL":
    import pickle
    with open(filename, "rb") as f:
      diag = pickle.load(f)
  else:
    raise InvalidArgument("extension " + filename.split('.')[-1] + " unknown. Please use among " + availableIDExts())

  diag.setProperty("name", diag.propertyWithDefault("name", ospath.splitext(ospath.basename(filename))[0]))
  return diag


def saveID(infdiag, filename):
  """
  save an ID into a file using the format corresponding to one of the availableWriteIDExts() suffixes.

  Parameters
  ----------
  infdiag : pyAgrum.InfluenceDiagram
    the Influence Diagram to save
  filename : str
    the name of the output file
  """
  extension = filename.split('.')[-1].upper()

  if extension in {"BIFXML", "BIFXML", "XML"}:
    infdiag.saveBIFXML(filename)
  elif extension == "PKL":
    import pickle
    with open(filename, "wb") as f:
      pickle.dump(infdiag, f, pickle.HIGHEST_PROTOCOL)
  else:
    raise InvalidArgument("extension " + filename.split('.')[-1] + " unknown. Please use among " + availableBNExts())


def fastID(structure, domain="[2]"):
  """
  Create an Influence Diagram with a modified dot-like syntax which specifies:
      - the structure and the type of the variables following :ref:`fast syntax<Quick specification of (randomly parameterized) graphical models>`,
      - a prefix for the type of node (chance/decision/utiliy nodes):

        - ``a`` : a chance node named 'a' (by default)
        - ``$a`` : a utility node named 'a'
        - ``*a`` : a decision node named 'a'

  Examples
  --------
  >>> import pyAgrum as gum
  >>> bn=gum.fastID('A->B[1,3]<-*C{yes|No}->$D<-E[1,2.5,3.9]',6)

  Parameters
  ----------
  structure : str
          the string containing the specification
  domain: int or str
          the default domain size (int) or domain specification (str) for variables (default is "[2]"

  Returns
  -------
  pyAgrum.InfluenceDiagram
          the resulting Influence Diagram
  """
  return InfluenceDiagram.fastPrototype(structure, domain)
%}
