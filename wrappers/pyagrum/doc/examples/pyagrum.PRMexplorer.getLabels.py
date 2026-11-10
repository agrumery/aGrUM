import pyagrum as gum

with open("office.o3prm", "w") as f:
  f.write("""
type t_state labels(OK, NOK);
type t_ink extends t_state (Full: OK, Low: OK, Empty: NOK);
interface Equipment { t_state state; }
class Printer implements Equipment {
  t_ink ink { [0.5, 0.3, 0.2] };
  t_state state dependson ink { [0.9, 0.8, 0.0, 0.1, 0.2, 1.0] };
}
class ColorPrinter extends Printer { boolean color { [0.1, 0.9] }; }
class Computer {
  param real lambda default 0.1;
  Printer[] printers;
  boolean canPrint = exists(printers.state, OK);
}
system office { Printer p; Computer c; c.printers += p; }
""")
ex = gum.PRMexplorer()
ex.load("./office.o3prm")
ex.getLabels("t_ink")
# ['Full', 'Low', 'Empty']
