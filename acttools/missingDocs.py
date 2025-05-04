############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import os
from os.path import join, dirname
import sys

from typing import Iterable
import types
import inspect

from .utils import notif

gumPath = join(dirname(sys.argv[0]), "build/pyAgrum/release/wrappers")


def _prefix(name: str) -> str:
    return " " * (4 * (name.count(".") - 1)) + "- "


class PyAgrumDocCoverage:
    def __init__(self, verbose: bool):
        self._verbose = verbose
        self.nb_class = 0
        self.nb_meth = 0
        self.nb_func = 0

        self.undoc_class = []
        self.undoc_func = []
        self.undoc_meth = []

        self.partial_doc_class = []
        self.partial_doc_func = []
        self.partial_doc_meth = []

        # preparecp the log file for missing documentations
        self.nodocfile = "nodoc.log"
        self.nodocfilebefore = "nodoc.before.log"
        if os.path.exists(self.nodocfile):
            if os.path.exists(self.nodocfilebefore):
                os.remove(self.nodocfilebefore)
            os.rename(self.nodocfile, self.nodocfilebefore)
        with open(self.nodocfile, "w") as f:
           # add the date to logfile
            from datetime import datetime
            f.write(datetime.now().strftime("%Y-%m-%d %H:%M:%S") + "\n")

    def lognodoc(self, msg: str):
        with open(self.nodocfile, "a") as f:
            f.write(msg + "\n")

    def notif_and_log(self, msg: str = "") -> None:
        notif(msg)
        self.lognodoc(msg)

    @staticmethod
    def _is_not_valid(msg: str) -> bool:
        # deprecated does not follow the rules of validity for documentation
        if msg.strip().startswith("Deprecated"):
            return False

        # msg les than 3 lines are not valid
        if msg.count("\n") < 3:
            return True

        if "'PyObject *'" in msg:
            return True

        return False

    def _check_function_oc(self, name: str, func: str):
        res = "check"
        is_meth = name.count(".") > 1

        skipped = {"gum.Potential", }
        if name in skipped:
            if self._verbose:
                notif(_prefix(name) + name + " : skipped")
            return

        if is_meth:
            self.nb_meth += 1
        else:
            self.nb_func += 1

        if not hasattr(func, "__doc__") or func.__doc__ is None:
            if is_meth:
                self.undoc_meth.append(name)
            else:
                self.undoc_func.append(name)
            res = "no doc"
        else:

            if self._is_not_valid(func.__doc__):
                if is_meth:
                    self.partial_doc_meth.append(name)
                else:
                    self.partial_doc_func.append(name)

                res = "partial"

        if res != "check":
            self.lognodoc(name + " : " + res)
        if self._verbose:
            notif(_prefix(name) + name + " : " + res)

    def _check_class_doc(self, name: str, clas: str):
        res = "check"
        self.nb_class += 1

        if not hasattr(clas, "__doc__") or clas.__doc__ is None:
            self.undoc_class.append(name)
            res = "no doc"
        else:
            if self._is_not_valid(clas.__doc__):
                self.partial_doc_class.append(name)
                res = "partial"

        if res != "check":
            self.lognodoc(name + " : " + res)
        if self._verbose:
            notif(_prefix(name) + name + " : " + res)

    @staticmethod
    def _ignored_class(clas: str) -> bool:
        if gumPath not in sys.path:
            sys.path.insert(0, gumPath)
        import pyagrum as gum

        return clas in {cls.__name__ for cls in gum.GumException.__subclasses__()}

    def _traversal(self, entities: Iterable[str], container: str):
        import pyagrum as gum
        for entity in entities:
            if entity[0] != '_':
                complete_entity_name = container + "." + entity
                instance_entity = eval(complete_entity_name)
                if type(instance_entity) is types.FunctionType:
                    self._check_function_oc(
                        complete_entity_name, instance_entity)
                elif inspect.isclass(instance_entity):
                    if not self._ignored_class(instance_entity.__name__):
                        self._check_class_doc(
                            complete_entity_name, instance_entity.__name__)
                        self._traversal(dir(instance_entity),
                                        complete_entity_name)

    def check_missing_doc(self):
        DELIM: str = "\n    + "

        if gumPath not in sys.path:
            sys.path.insert(0, gumPath)
        import pyagrum as gum

        self.nb_class = 0
        self.nb_meth = 0
        self.nb_func = 0

        self.undoc_class = []
        self.undoc_func = []
        self.undoc_meth = []

        self.partial_doc_class = []
        self.partial_doc_func = []
        self.partial_doc_meth = []

        self._traversal(dir(gum), "gum")

        pc = 1.0 - (len(self.undoc_class) +
                    len(self.partial_doc_class)) / (1.0 * self.nb_class)
        pm = 1.0 - (len(self.undoc_meth) +
                    len(self.partial_doc_meth)) / (1.0 * self.nb_meth)
        pf = 1.0 - (len(self.undoc_func) +
                    len(self.partial_doc_func)) / (1.0 * self.nb_func)

        tc = self.nb_class - (len(self.undoc_class) +
                              len(self.partial_doc_class))
        tm = self.nb_meth - (len(self.undoc_meth) + len(self.partial_doc_meth))
        tf = self.nb_func - (len(self.undoc_func) + len(self.partial_doc_func))

        self.lognodoc("\n\n"+"="*50)
        self.notif_and_log(f'Documentation in pyAgrum {gum.__version__}')

        self.notif_and_log(
            f"  Classes   : coverage={(pc * 100.0):6.2f}% [({tc}/{self.nb_class})]")
        if self._verbose:
            self.notif_and_log("---------")
            self.notif_and_log("  - nbr of classes : " + str(self.nb_class))
            self.notif_and_log(
                "  - nbr of partially documented classes : " + str(len(self.partial_doc_class)))
            self.notif_and_log(DELIM.join([""] + self.partial_doc_class))
            self.notif_and_log()
            self.notif_and_log(
                "  - nbr of undocumented classes : " + str(len(self.undoc_class)))
            self.notif_and_log(DELIM.join([""] + self.undoc_class))

        self.notif_and_log(
            f"  Methods   : coverage={(pm * 100.0):6.2f}% [({tm}/{self.nb_meth})]")
        if self._verbose:
            self.notif_and_log("---------")
            self.notif_and_log("  - nbr of methods: " + str(self.nb_meth))
            self.notif_and_log(
                "  - nbr of partially documented methods : " + str(len(self.partial_doc_meth)))
            self.notif_and_log(DELIM.join([""] + self.partial_doc_meth))
            self.notif_and_log()
            self.notif_and_log(
                "  - nbr of undocumented methods : " + str(len(self.undoc_meth)))
            self.notif_and_log(DELIM.join([""] + self.undoc_meth))

        self.notif_and_log(
            f"  Functions : coverage={(pf * 100.0):6.2f}% [({tf}/{self.nb_func})]")
        if self._verbose:
            self.notif_and_log("-----------")
            self.notif_and_log("  - nbr of functions: " + str(self.nb_func))
            self.notif_and_log(
                "  - nbr of partially documented functions : " + str(len(self.partial_doc_func)))
            self.notif_and_log(DELIM.join([""] + self.partial_doc_func))
            self.notif_and_log()
            self.notif_and_log(
                "  - nbr of undocumented functions : " + str(len(self.undoc_func)))
            self.notif_and_log(DELIM.join([""] + self.undoc_func))
        self.lognodoc("="*50)

        return (len(self.undoc_class) +
                len(self.partial_doc_class) +
                len(self.undoc_meth) +
                len(self.partial_doc_meth) +
                len(self.undoc_func) +
                len(self.partial_doc_func))


def missingDocs(show_funct: bool = False):
    return PyAgrumDocCoverage(verbose=show_funct).check_missing_doc()


if __name__ == "__main__":
    # execute only if run as a script
    self.notif_and_log(
        f"\nNbr of documentation errors: {missingDocs(show_funct=True)}")
