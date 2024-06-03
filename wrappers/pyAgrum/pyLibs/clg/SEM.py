# -*- coding: utf-8 -*-
# (c) Copyright 2022-2023 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

"""
This module contains the SEM class, which is used to parse a SEM into a CLG model or convert a CLG model into a SEM.
"""

import re
import warnings

from .CLG import CLG
from .GaussianVariable import GaussianVariable


class SEM:
  """
  This class is used to parse a SEM into a CLG model or convert a CLG model into a SEM.

  code
  ----

  sem = SEM('''
  # hyper parameters
  A = 4[5]
  B = 3[5]
  C = -2[5]

  # equations
  D = A[.2]  # D is a noisy version of A
  E = 1 + D + 2
  B[2]
  F = E + C + B + E[0.001]
  ''')
  """

  NUMBER = r"[0-9]*\.?[0-9]*"
  ID = r"[a-zA-Z_]\w*"
  FIND_VAR = rf"^({ID})$"
  FIND_STDDEV = rf"^\[({NUMBER})\]$"
  FIND_FLOAT = rf"^({NUMBER})$"
  FIND_TERM = rf"^({NUMBER})({ID})$"

  _reg_variable = re.compile(FIND_VAR)
  _reg_stddev = re.compile(FIND_STDDEV)
  _reg_term = re.compile(FIND_TERM)
  _reg_float = re.compile(FIND_FLOAT)

  def __init__(self):
    """
    Constructor
    """
    warnings.warn("SEM has not to be instantiated.")

  @staticmethod
  def _check_token(s, pos, pattern):
    """
    Check if the string s[pos:] matches the pattern.
    If it does, return the match and the position of the next character.
    If it does not, return None, None.

    Parameters
    ----------
    s : str
      The string to be checked.
    pos : int
      The position in the string where to start the check.
    pattern : re.Pattern
      The pattern to be matched.

    Returns
    -------
    Tuple[str, int] or Tuple[None, None]
      The match and the position of the next character if the string matches the pattern.
    """
    res = pattern.match(s[pos:])
    if res is None:
      return None, None
    else:
      return res.group(1), res.end()

  @staticmethod
  def _parse_line(s: str):
    """
    This function parses a line of a SEM.

    Parameters
    ----------
    s : str
      The line to be parsed.

    Returns
    -------
    Tuple[str, Dict[str, str | float], float]
      The name of the variable, the dictionary of the terms and the standard deviation.
    """

    ss = s.split("#")[0]
    ss = "".join(ss.split())
    # allow every tokens to be separated with "+"
    ss = ss.replace("-+", "-")
    ss = ss.replace("-", "+-")
    ss = ss.replace("=", "=+")
    ss = ss.replace("[", "+[")

    # to be sure there are now empty tokens
    tokens = [s for s in ss.split("+") if s != ""]

    token = tokens.pop()
    if SEM._reg_stddev.match(token) is None:
      raise SyntaxError(f"{token=} not recognized as a stdev")
    stddev = token[1:-1]

    tokens.reverse()
    token = tokens.pop()
    if token[-1] != "=":
      raise SyntaxError("No affectation operator ('=')")
    variable = token[:-1]
    if SEM._reg_variable.match(variable) is None:
      raise SyntaxError(f"{variable=} not a variable")

    terms = {}
    for token in tokens:
      # get rid of "-" at the beginning of token
      if token[0] == "-":
        coef = -1
        token = token[1:]
      else:
        coef = 1

      if (m := SEM._reg_variable.match(token)) is not None:
        var = m.group(0)
      elif (m := SEM._reg_float.match(token)) is not None:
        coef *= float(m.group(0))
        var = ""
      elif (m := SEM._reg_term.match(token)) is not None:
        coef *= float(m.group(1))
        var = m.group(2)
      else:
        break

      # allow more than one term for the same variable (or more than one constant)
      if var in terms:
        terms[var] += coef
      else:
        terms[var] = coef

    if len(terms) > len(tokens):
      raise SyntaxError("Some tokens not recognized")
    if len(terms) + 2 < len(tokens):
      raise SyntaxError("Tokens after stddev")

    return variable, terms, float(stddev)

  @staticmethod
  def toclg(sem: str) -> CLG:
    """
    This function parses a SEM into a CLG model.

    Parameters
    ----------
    sem : str
      The SEM to be parsed.

    Returns
    -------
    CLG
      The CLG model corresponding to the SEM.
    """

    canonic_form = []
    for line in re.split('\n+', sem):
      line = line.strip()
      if line == "":
        continue
      if line[0] == "#":
        continue
      if "=" in line:  # new equation
        canonic_form.append(line)
      else:
        if len(canonic_form) == 0:
          raise SyntaxError(f"Line '{line}' is not an equation.")
        canonic_form[-1] += line

    # parsing the canonic_form
    clg = CLG()
    arcs = []
    arcs = []
    for line in canonic_form:
      try:
        var, terms, stddev = SEM._parse_line(line)
      except SyntaxError as e:
        raise SyntaxError(f"In line {line}, {e.msg()}.")
      mu = 0
      for parent in terms:
        if parent == "":
          mu = terms[""]
        else:
          arcs.append((parent, var, terms[parent]))
      clg.add(GaussianVariable(var, mu, stddev))
    for head, tail, coef in arcs:
      clg.addArc(head, tail, coef)

    return clg

  @staticmethod
  def tosem(clg: CLG) -> str:
    """
    This function converts a CLG model into a SEM.

    Parameters
    ----------
    clg : CLG
      The CLG model to be converted.

    Returns
    -------
    lines : str
      The SEM corresponding to the CLG model.
    """

    lines = ""
    for node in clg.topologicalOrder():
      line = ""

      # after the "="
      for parent in clg.parents(node):
        if clg._arc2coef[(parent, node)] == 1:
          line += f"+{clg._id2var[parent].name()}"
        elif clg._arc2coef[(parent, node)] < 0:
          line += f"-{-clg._arc2coef[(parent, node)]}{clg._id2var[parent].name()}"
        else:
          line += f"+{clg._arc2coef[(parent, node)]}{clg._id2var[parent].name()}"
      line += f"[{clg._id2var[node].sigma()}]"

      # before the "="
      line = clg._id2var[node].name() + "=" + str(clg._id2var[node].mu()) + line
      line = line.replace("=0+", "=")  # remove the 0

      lines += line + "\n"

    return lines

  @staticmethod
  def saveCLG(clg: CLG, filename: str):
    """
    Save the CLG as a SEM to a file.

    Parameters
    ----------
    clg : CLG
      The CLG model to be saved.
    filename : str
      The name of the file containing the SEM of CLG.
    """
    with open(filename, 'w') as file:
      file.write(SEM.tosem(clg))

  @staticmethod
  def loadCLG(filename: str) -> CLG:
    """
    Load the CLG from the file containing a SEM.

    Parameters
    ----------
    filename : str
      The name of the file containing the SEM of CLG.

    Returns
    -------
    the loaded CLG
    """
    with open(filename, 'r') as file:
      sem = file.read()

    return SEM.toclg(sem)
