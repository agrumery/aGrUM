from configparser import ConfigParser
import os


class PyAgrumConfiguration:
  """ PyAgrumConfiguration is a the pyAgrum configuration singleton (not a singleton for now :-( )
  """

  def __init__(self, defaults):
    self.__parser=ConfigParser(allow_no_value=False)
    self.__defaults=defaults
    self.__parser.read_string(defaults)

  def set(self, section, option, value):
    """set a property in a section
    
    Arguments:
        section {str} -- the section name (has to exist in defaults)
        option {str} -- the option/property name (has to exist in defaults)
        value {str} -- the value (will be store as string)
    
    Raises:
        SyntaxError: if the secion name or the property name does not exist
    """
    if section in self.__parser.sections():
      if option in self.__parser[section]:
        self.__parser.set(section, option, str(value))
        return

    raise SyntaxError("You can not add option '"+section +
                      "."+option+"' in pyAgrum configuration")

  def get(self,section,option):
    """ Give the value associated to section.option
    
    Arguments:
        section {str} -- the section
        option {str} -- the property
    
    Returns:
        str -- the value (as string)
    """
    return self.__parser.get(section,option)

  def __diff(self):
    mine=self.__parser
    c = ConfigParser()
    c.read_string(self.__defaults)
    def aff_sec(section): return "["+section+"]\n"+"\n".join(
        ["  {} = {}".format(key, mine[section][key]) for key in mine[section].keys() if mine.get(section,key)!=c.get(section,key)])
    return "\n".join([aff_sec(section) for section in mine.sections()])

  def save(self):
    """Save the diff with the defaults in 'pyagrum.ini' in current directory
    """
    with open("pyagrum.ini", "w") as configfile:
      print(self.__diff(),file=configfile)

  def reset(self):
    """ back to defaults
    """
    self.__parser.read_string(self.__defaults)

  def load(self):
    """load pyagrum.ini and change the properties if needed
    
    Raises:
        FileNotFoundError: if there is no pyagrum.ini in the current directory
    """
    if os.path.isfile("pyagrum.ini"):
      # to force the use of the protected set() method
      c = ConfigParser()
      c.read("pyagrum.ini")
      for section in c.sections():
        for option in c[section]:
          self.set(section, option, c[section][option])
    else:
      raise FileNotFoundError("No file 'pyagrum.ini' in current directory.")

  def grep(self, search):
    """ grep in the configuration any section or properties matching the argument. If a section match the argume, all the section is displayed.
    
    Arguments:
        search {str} -- the string to find    
    """
    mine=self.__parser
    lowsearch = search.lower()
    def aff_sec(section,all): return "["+section+"]\n"+"\n".join(
        ["  {} = {}".format(key, mine[section][key]) for key in mine[section].keys() if all or lowsearch in key])
    print("\n".join([aff_sec(section,lowsearch in section) for section in mine.sections()]))

  def diff(self):
    """ print the diff between actual configuration and the defaults. This is what is saved in the file 'pyagrum.ini' by the method `PyAgrumConfiguration.save()`
    """
    print(self.__diff())

  def __str__(self):
    mine=self.__parser
    def aff_sec(section): return "["+section+"]\n"+"\n".join(
        ["  {} = {}".format(key, mine[section][key]) for key in mine[section].keys()])
    return "\n".join([aff_sec(section) for section in mine.sections()])

  def __repr__(self):
    res=self.__diff()
    if "=" in res:
      return res
    else:
      return "# no customized property\n"+self.__str__()
