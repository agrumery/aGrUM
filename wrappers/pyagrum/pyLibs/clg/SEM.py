############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

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

  Each equation reads ``name = terms [stddev]``: the terms are a constant, a variable or a coefficient times a
  variable (``2.5*A``, ``2.5 A`` or ``2.5A``), separated by ``+`` or ``-``, and the standard deviation of the noise is
  written between brackets at the end. The spaces separate the tokens. The numbers may use the exponent notation
  (``1e-05``), the exponent being glued to its number: ``2e3A`` is 2000 times ``A``, ``2 e3`` or ``2*e3`` is 2 times a
  variable ``e3``. ``#`` starts a comment. A line without ``=`` (outside its comment) continues the previous equation
  and must start with ``+``, ``-`` or ``[``.

  Examples
  --------
  >>> sem = SEM.toclg('''
  ... # hyper parameters
  ... A = 4[5]
  ... B = 3[5]
  ... C = -2[5]
  ...
  ... # equations
  ... D = A[.2]  # D is a noisy version of A
  ... E = 1 + D
  ...     + 2*B[2]  # a line without equal sign continues the equation
  ... F = E + C + 3.5*B + E[0.001]  # the terms of a same variable are summed: 2*E
  ... ''')
  >>> print(SEM.tosem(sem))
  C=-2.0[5.0]
  B=3.0[5.0]
  A=4.0[5.0]
  D=A[0.2]
  E=1.0+2.0*B+D[2.0]
  F=3.5*B+C+2.0*E[0.001]
  """

  NUMBER = r"(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[eE][+-]?[0-9]+)?"
  ID = r"[a-zA-Z_]\w*"
  FIND_VAR = rf"^({ID})$"
  FIND_STDDEV = rf"^\[({NUMBER})\]$"
  FIND_FLOAT = rf"^({NUMBER})$"
  FIND_TERM = rf"^({NUMBER})\*?({ID})$"

  _reg_variable = re.compile(FIND_VAR)
  _reg_stddev = re.compile(rf"\s*\[\s*({NUMBER})\s*\]")
  # a term: signs, then a number possibly followed by a variable, or a variable alone; the spaces separate the tokens
  # (an exponent is glued to its number: '2e3' is a number, '2 e3' is 2 times e3)
  _reg_term = re.compile(rf"\s*((?:[+-]\s*)*)(?:({NUMBER})(?:\s*\*?\s*({ID}))?|({ID}))")

  def __init__(self):
    """
    Constructor
    """
    warnings.warn("SEM has not to be instantiated.")

  @staticmethod
  def _check_token(s: str, pos: int, pattern: re.Pattern[str]) -> tuple[str, int] | tuple[None, None]:
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
    tuple[str, int] or tuple[None, None]
      The match and the position of the next character if the string matches the pattern.
    """
    res = pattern.match(s[pos:])
    if res is None:
      return None, None
    else:
      return res.group(1), res.end()

  @staticmethod
  def _parse_line(s: str) -> tuple[str, dict[str, float], float]:
    """
    This function parses a line of a SEM.

    Parameters
    ----------
    s : str
      The line to be parsed.

    Returns
    -------
    tuple[str, dict[str, float], float]
      The name of the variable, the dictionary of the terms (the key "" for the constant) and the standard deviation.

    Raises
    ------
    SyntaxError
      If the line does not follow the syntax of a SEM.
    """
    ss = s.split("#")[0]
    if "=" not in ss:
      raise SyntaxError("No affectation operator ('=')")
    variable, rhs = (part.strip() for part in ss.split("=", 1))
    if SEM._reg_variable.match(variable) is None:
      raise SyntaxError(f"{variable=} not a variable")

    terms: dict[str, float] = {}
    pos = 0
    while True:
      if (m := SEM._reg_stddev.match(rhs, pos)) is not None:
        if m.end() != len(rhs):
          raise SyntaxError(f"Tokens after stddev: '{rhs[m.end() :]}'")
        return variable, terms, float(m.group(1))

      m = SEM._reg_term.match(rhs, pos)
      # a term other than the first one starts with a sign
      if m is None or m.end() == pos or (pos > 0 and m.group(1) == ""):
        if pos == len(rhs):
          raise SyntaxError("No stddev ('[...]') at the end of the equation")
        raise SyntaxError(f"'{rhs[pos:]}' not recognized")

      coef = -1.0 if m.group(1).count("-") % 2 == 1 else 1.0
      if m.group(4) is not None:  # a variable alone
        var = m.group(4)
      else:  # a number, possibly followed by a variable
        coef *= float(m.group(2))
        var = m.group(3) if m.group(3) is not None else ""

      # allow more than one term for the same variable (or more than one constant)
      terms[var] = terms.get(var, 0.0) + coef
      pos = m.end()

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
    for line in re.split("\n+", sem):
      line = line.split("#")[0].strip()  # comments never decide nor hide anything
      if line == "":
        continue
      if "=" in line:  # new equation
        canonic_form.append(line)
      else:
        if len(canonic_form) == 0:
          raise SyntaxError(f"Line '{line}' is not an equation.")
        # a number ending a line and a name starting the next one would read as a term (2 and B: 2 B)
        if line[0] not in "+-[":
          raise SyntaxError(f"Line '{line}' continues an equation: it must start with '+', '-' or '['.")
        canonic_form[-1] += " " + line

    # parsing the canonic_form
    clg = CLG()
    arcs: list[tuple[str, str, float, str]] = []
    for line in canonic_form:
      try:
        var, terms, stddev = SEM._parse_line(line)
      except SyntaxError as e:
        raise SyntaxError(f"In line {line}, {e.args[0]}.")
      mu = 0.0
      for parent in terms:
        if parent == "":
          mu = terms[""]
        else:
          arcs.append((parent, var, terms[parent], line))
      clg.add(GaussianVariable(var, mu, stddev))
    names = set(clg.names())
    for head, tail, coef, line in arcs:
      if head not in names:
        raise SyntaxError(f"In line {line}, unknown variable '{head}'.")
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
      # "coef*name", so that the coefficient and the name never merge (2 times e3 is not 2e3)
      for parent in clg.parents(node):
        coef = clg._arc2coef[(parent, node)]
        name = clg._id2var[parent].name()
        if coef == 1:
          line += f"+{name}"
        elif coef == -1:
          line += f"-{name}"
        elif coef < 0:
          line += f"-{-coef}*{name}"
        else:
          line += f"+{coef}*{name}"
      line += f"[{clg._id2var[node].sigma()}]"

      # before the "="
      line = clg._id2var[node].name() + "=" + str(clg._id2var[node].mu()) + line
      line = line.replace("=0.0+", "=").replace("=0+", "=")

      lines += line + "\n"

    return lines

  @staticmethod
  def saveCLG(clg: CLG, filename: str) -> None:
    """
    Save the CLG as a SEM to a file.

    Parameters
    ----------
    clg : CLG
      The CLG model to be saved.
    filename : str
      The name of the file containing the SEM of CLG.
    """
    with open(filename, "w") as file:
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
    with open(filename) as file:
      sem = file.read()

    return SEM.toclg(sem)
