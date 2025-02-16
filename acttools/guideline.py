############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
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
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
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
from datetime import datetime

from subprocess import call

from .utils import notif, error, recglob, srcAgrum, srcSwigAgrum, srcPyAgrum, srcSwigAgrum, notif_oneline
from .configuration import cfg

from .missingDocs import missingDocs
from .checkDependencies import check_gum_dependencies


def guideline(current: dict[str, str], correction=False) -> int:
    def _aff_errors(nb: int, typ: str) -> int:
        if nb > 0:
            # spaces to remove others possible characters
            error(f"{nb} {typ} error{'s' if nb > 1 else ''}{' ' * 40}")
        return nb

    if correction:
        notif("[aGrUM guideline (with correction)]")
    else:
        notif("[aGrUM guideline]")

    nbrError = 0

    notif("  [(1) ]*.cpp[ file for every ]*.h[ file]")
    nbrError += _aff_errors(_checkCppFileExists(correction),
                            "missing cpp file")
    notif("  [(2) check for ]LGPL+MIT[ license]")
    nbrError += _aff_errors(_checkLGPL_MIT_license_CPP(correction),
                            "missing LGPL+MIT licence")
    nbrError += _aff_errors(_checkLGPL_MIT_license_py(correction),
                            "missing LGPL+MIT licence")
    notif("  [(3) check for missing documentation in pyAgrum]")
    nbrError += _aff_errors(_checkMissingDocs(correction),
                            "missing documentation")
    notif("  [(4) check for deps]")
    nbrError += _aff_errors(check_gum_dependencies(graph=current['build_graph'], correction=correction),
                            "redundant dependency")
    notif("  [(5) check for format]")
    nbrError += _aff_errors(_checkClangFormat(correction), "format")

    return nbrError


def _checkClangFormat(correction: bool) -> int:
    nbrError = 0
    if cfg.clangformat is None:
        error("No correct [clang-format] tool has been found.")
    else:
        with open(os.devnull, "w") as blackhole:
            for src in srcAgrum():
                exceptions = {f'{os.sep}external{os.sep}', 'Parser', 'Scanner'}
                if any(subs in src for subs in exceptions):
                    continue

                line = cfg.clangformat + " " + src + " | cmp " + src + " -"
                if call(line, shell=True, stderr=blackhole, stdout=blackhole) == 1:
                    nbrError += 1
                    if correction:
                        line = cfg.clangformat + " -i " + src
                        call(line, shell=True)
                        notif(f"Incorrect format [{src:80}] : [(✓)]")
                    else:
                        notif(f"Incorrect format [{src}]")
                else:
                    notif_oneline(f"[{src.split('/')[-1]}] OK")

    return nbrError


def _LGPL_MIT_atTop_CPP(filename: str, correction: bool) -> int:
    before = licence = code = ""

    state = "before"  # before->inComment->after
    in_error = False

    with open(filename, "r") as origin:
        while line := origin.readline():
            if line.strip() == "":
                if state == "before":
                    if before != "":
                        before += line
                elif state == "inComment":
                    licence += line
                else:
                    code += line
                continue

            if state == "before":
                if line.startswith("%feature"):
                    state = "after"
                    code += line
                elif not line.startswith("/**"):
                    if not in_error:
                        in_error = True
                        notif(f"[{filename}] lines before the license.")
                    before += line
                else:
                    licence += line
                    state = "inComment"
            elif state == "inComment":
                licence += line
                if line.strip().endswith("*/"):
                    state = "after"
            else:  # state == "after"
                code += line

        err = 0
        if filename.endswith("bdd.h"):
            print(f"{base=} {licence=}")
        if licence.strip() != _template_cpp_license.strip():
            err = 1
            res = f"[{filename:.<80}] missing up-to-date LGPL+MIT license"
            if correction:
                with open(filename, "w") as dest:
                    dest.write(before)
                    dest.write(_template_cpp_license)
                    dest.write(code)
                res = f"{res} [(✓)]"

            notif(res)

    return err


def _LGPL_MIT_atTop_py(filename: str, correction: bool) -> int:
    before = licence = code = ""

    state = "before"  # before->inComment->after
    in_error = False
    with open(filename, "r") as origin:
        while line := origin.readline():
            if line.strip() == "":
                if state == "before":
                    if before != "":
                        before += line
                elif state == "inComment":
                    licence += line
                else:
                    code += line
                continue

            if state == "before":
                if line[0] != "#":
                    if not in_error:
                        in_error = True
                        notif(f"[{filename}] lines before the license.")
                    before += line
                else:
                    licence += line
                    state = "inComment"
            elif state == "inComment":
                if line[0] != "#":
                    state = "after"
                    code += line
                else:
                    licence += line
            else:  # state == "after"
                code += line

    err = 0
    if licence != _template_py_license:
        err = 1
        res = f"[{filename:.<80}] missing up-to-date LGPL+MIT license"
        if correction:
            with open(filename, "w") as dest:
                dest.write(before)
                dest.write(_template_py_license)
                dest.write(code)
            res = f"{res} [(✓)]"
        notif(res)

    return err


def _checkLGPL_MIT_license_CPP(correction: bool) -> int:
    nbrError = 0

    exceptions = [f'{os.sep}mvsc{os.sep}',
                  f'{os.sep}external{os.sep}',
                  f'{os.sep}cxxtest{os.sep}',
                  'Parser', 'Scanner']
    for gum_file in srcAgrum():
        if any(subs in gum_file for subs in exceptions):
            notif(f"skip header test for [{gum_file}]")
            continue
        nbrError += _LGPL_MIT_atTop_CPP(gum_file, correction)
    for gum_file in srcSwigAgrum():
        if any(subs in gum_file for subs in exceptions):
            notif(f"skip header test for [{gum_file}]")
            continue
        nbrError += _LGPL_MIT_atTop_CPP(gum_file, correction)

    return nbrError


def _checkLGPL_MIT_license_py(correction: bool) -> int:
    nbrError = 0

    exceptions = []
    for pygum_file in srcPyAgrum():
        notif_oneline(f"[{pygum_file.split('/')[-1]}]")
        if any(subs in pygum_file for subs in exceptions):
            continue
        nbrError += _LGPL_MIT_atTop_py(pygum_file, correction)

    return nbrError


def _checkCppFileExists(correction: bool) -> int:
    nbrError = 0

    exceptions = [f'{os.sep}mvsc{os.sep}', f'{os.sep}signal{os.sep}', f'{os.sep}external{os.sep}',
                  f'multidim{os.sep}patterns{os.sep}',
                  'agrum.h', 'inline.h', 'base.h', 'bn.h', 'cn.h', 'id.h', 'mrf.h',
                  f'MN{os.sep}MarkovNet.h', f'MN{os.sep}inference{os.sep}ShaferShenoyMNInference.h']

    for header in recglob(f"src{os.sep}agrum", "*.h"):
        if any(subs in header for subs in exceptions):
            print(f"skip {header}")
            continue

        subs = header[:-1]
        if subs.endswith("_tpl."):
            continue
        if subs.endswith("_inl."):
            continue
        cppfile = subs + "cpp"
        if not os.path.isfile(cppfile):
            nbrError += 1
            if correction:
                with open(cppfile, "w") as dest:
                    dest.write(_template_cpp_license)
                    dest.write(f"\n#include \"{header[4:]}\"\n")
                error("No cpp file for [" + header + "h] : [added]")
            else:
                error("No cpp file for [" + header + "h]")

    return nbrError


def _checkMissingDocs(correction: bool) -> int:
    return missingDocs(correction)


def getTemplateLicense() -> tuple[str, str]:
    """
    Get the template license for python and c++ files
    """
    current_year = datetime.now().year

    template_license = f"""
  This file is part of the aGrUM/pyAgrum library.

  Copyright (c) 2005-{current_year} by
      - Pierre-Henri WUILLEMIN(_at_LIP6)
      - Christophe GONZALES(_at_AMU)
  This file is part of the aGrUM/pyAgrum library.

  Copyright (c) 2005-{current_year} by
      - Pierre-Henri WUILLEMIN(_at_LIP6)
      - Christophe GONZALES(_at_AMU)

  The aGrUM/pyAgrum library is free software; you can redistribute it
  and/or modify it under the terms of either :

   - the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License,
     or (at your option) any later version.
   - the MIT license (MIT)
   - or both in dual license, as here

  This aGrUM/pyAgrum library is distributed in the hope that it will be
  useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT
  licence (LICENSE.MIT) for more details.

  Contact  : info_at_agrum_dot_org
  homepage : http://agrum.gitlab.io
  gitlab   : https://gitlab.com/agrumery/agrum
      """
    width = max(len(line.rstrip()) for line in template_license.splitlines())

    template_py_license = "#" * (width + 4) + "\n"
    template_cpp_license = "/" + ("*" * (width + 4)) + "\n"

    for n, l in enumerate(template_license.splitlines()):
        if n == 0 and l.strip() == "":
            continue
        template_py_license += f"# {l.rstrip():{width}} #\n"
        template_cpp_license += f" * {l.rstrip():{width}} *\n"

    template_py_license += "#" * (width + 4) + "\n\n"
    template_cpp_license += " " + "*" * (width + 4) + "/\n\n"

    return template_py_license, template_cpp_license


_template_py_license, _template_cpp_license = getTemplateLicense()
