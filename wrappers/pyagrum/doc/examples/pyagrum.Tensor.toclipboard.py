# requires: pandas
import pandas as pd

import pyagrum as gum

a, b = gum.LabelizedVariable("a", "a", 2), gum.LabelizedVariable("b", "b", 3)
p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6])
# the table can then be pasted in a spreadsheet
try:
  p.toclipboard()
# no system clipboard: PyperclipException on a server without display, AttributeError on macOS without session
except (pd.errors.PyperclipException, AttributeError):
  pass
