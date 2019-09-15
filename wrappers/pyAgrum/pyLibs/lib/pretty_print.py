# !/usr/bin/python
# -*- coding: utf-8 -*-

# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017  (pierre-henri.wuillemin@lip6.fr)

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

from __future__ import print_function
import sys
import os

from ._utils.pyAgrum_header import pyAgrum_header
import pyAgrum as gum

DECIMAL_LENGTH = 4
BLANK = ' '


def module_help(exit_value=1):
    """
    defines help viewed if args are not OK on command line, and exit with exit_value
    """
    print(os.path.basename(sys.argv[0]), "src.{" + gum.availableBNExts() + "}")
    sys.exit(exit_value)


def max_length(v):
    m = len(v.name())
    for i in range(v.domainSize()):
        m = max(m, len(v.label(i)))
    return m


def cpt2txt(cpt,digits=DECIMAL_LENGTH):
    """
    string representation of a gum.Potential

    :param cpt: the Potential to represent
    :return: the string representation
    """
    number_format = '{0:.' + str(digits) + 'f}'
    res = []

    size = (1 + 2 + digits) * cpt.variable(0).domainSize() - 1

    width = {}
    total_width = 0
    for j in range(1, cpt.nbrDim()):
        width[j] = 2 + max_length(cpt.variable(j))
        total_width += width[j] + 1
    total_width -= 1

    res.append(' ' * total_width + '|-' + '-' * size + '-|')

    line = '-' * total_width + '|'
    line += "{0:^{1}}".format(cpt.variable(0).name()[0:size + 2], size + 2)
    line += '|'
    res.append(line)

    line = ''
    for j in range(1, cpt.nbrDim()):
        line += "{0:^{1}}|".format(cpt.variable(j).name(), width[j])
    if line == '':
        line = '|'
    line += BLANK
    for j in range(cpt.variable(0).domainSize()):
        line += ("{0:^{1}}" + BLANK).format(cpt.variable(0).label(j)[0:digits + 2], digits + 2)
    line += '|'
    res.append(line)
    res.append(('-' * total_width) + '|' + '-' * (2 + size) + '|')

    i = gum.Instantiation(cpt)
    i.setFirst()
    while not (i.end()):
        line = ''
        for j in range(1, cpt.nbrDim()):
            line += "{0:^{1}}|".format(cpt.variable(j).label(i.val(j)), width[j])
        if line == '':
            line = '|'
        line += ' '
        for j in range(cpt.variable(0).domainSize()):
            line += number_format.format(cpt.get(i)) + ' '
            i.inc()
        line += '|'
        res.append(line)

    res.append(('-' * total_width) + '|' + '-' * (2 + size) + '|')
    return "\n".join(res)


def bn2txt(aBN):
    """
    Representation of all CPTs of a gum.BayesNet

    :param aBN: the bayes net or the name of the file
    :return:
    """
    if isinstance(aBN, str):
        bn = gum.loadBN(aBN)
    else:
        bn = aBN

    return "\n".join([cpt2txt(bn.cpt(nod)) for nod in bn.topologicalOrder()])


if __name__ == "__main__":
    pyAgrum_header("2011-19")

    if len(sys.argv) < 2:
        module_help()

    bn2txt(sys.argv[1])
