#!/usr/bin/python
# -*- coding: utf-8 -*-

#(c) Copyright by Pierre-Henri Wuillemin, UPMC, 2012  (pierre-henri.wuillemin@lip6.fr)

#Permission to use, copy, modify, and distribute this
#software and its documentation for any purpose and
#without fee or royalty is hereby granted, provided
#that the above copyright notice appear in all copies
#and that both that copyright notice and this permission
#notice appear in supporting documentation or portions
#thereof, including modifications, that you make.

#THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
#WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
#SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
#OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
#RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
#IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
#ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
#OR PERFORMANCE OF THIS SOFTWARE!

"""
tools for BN analysis in ipython qtconsole and notebook

(but can be used every where)
"""
from base64 import encodestring
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg as fc

import pydot

import IPython.display
from IPython.core.pylabtools import print_figure
import pyAgrum as gum
import numpy as np

def getPosterior(bn,ev,target):
    """
    Compute the posterior of a single target (variable) in a BN given evidence using Lazy Propagation (for now).
    """
    inf=gum.LazyPropagation(bn)
    inf.setEvidence(ev)
    inf.makeInference()
    return inf.marginal(bn.idFromName(target))

def showProba(p):
    """
    Show a matplotlib barh (horizontal histogram) for a Potential p.

    """
    var=p.variable(0)
    ra=np.arange(len(var))
    vx=[var.label(int(i)) for i in ra]

    fig=plt.figure()
    fig.set_figheight(len(var)/4.0)
    ax=fig.add_subplot(111)

    ax.barh(ra,p.tolist(),align='center')
    ax.set_yticks(ra)
    ax.set_yticklabels(vx)
    ax.set_xlabel('Probability')
    ax.set_title(var.name())
    ax.get_xaxis().grid(True)
    plt.show()

def showPosterior(bn,ev,target):
    """
    shortcut for showProba(getPosterior(bn,ev,target))
    """
    showProba(getPosterior(bn,ev,target))

def showBN(bn,size="4"):
    """
    Shows a graphviz svg representation of the BN using size ("1" ,"2" , ..., "6")
    """
    import pydot
    import IPython.core.display

    p=pydot.graph_from_dot_data(bn.toDot())
    p.set_size(size)
    i=IPython.core.display.SVG(p.create_svg())
    IPython.core.display.display(i)
    
def animApproximationScheme(apsc):
  """
  show an animated version of an approximation scheme
  """
  from IPython.display import clear_output,display
  f=plt.gcf()
  
  h=gum.PythonApproximationListener(apsc)
  apsc.setVerbosity(True)
  apsc.listener=h
  
  def stopper(x):
    clear_output()
    print("Solutions : {0}".format(x))
    print("Time : {0} s".format(apsc.currentTime()))
    print("Iterations : {0}".format(apsc.nbrIterations()))
    
  def progresser(x,y,z):
    if len(apsc.history())<10:
      plt.xlim(1,10)
    else:
      plt.xlim(1,len(apsc.history()))
    plt.plot(np.log10(apsc.history()), 'g')
    clear_output()
    display(f)
          
  h.setWhenStop(stopper)
  h.setWhenProgress(progresser)
  
 
    
def xkcd_line(x, y, xlim=None, ylim=None,
              mag=1.0, f1=30, f2=0.05, f3=15):
    """
    Mimic a hand-drawn line from (x, y) data

    Parameters
    ----------
    x, y : array_like
        arrays to be modified
    xlim, ylim : data range
        the assumed plot range for the modification.  If not specified,
        they will be guessed from the  data
    mag : float
        magnitude of distortions
    f1, f2, f3 : int, float, int
        filtering parameters.  f1 gives the size of the window, f2 gives
        the high-frequency cutoff, f3 gives the size of the filter
    
    Returns
    -------
    x, y : ndarrays
        The modified lines
    """
    from scipy import interpolate, signal
    
    x = np.asarray(x)
    y = np.asarray(y)
    
    # get limits for rescaling
    if xlim is None:
        xlim = (x.min(), x.max())
    if ylim is None:
        ylim = (y.min(), y.max())

    if xlim[1] == xlim[0]:
        xlim = ylim
        
    if ylim[1] == ylim[0]:
        ylim = xlim

    # scale the data
    x_scaled = (x - xlim[0]) * 1. / (xlim[1] - xlim[0])
    y_scaled = (y - ylim[0]) * 1. / (ylim[1] - ylim[0])

    # compute the total distance along the path
    dx = x_scaled[1:] - x_scaled[:-1]
    dy = y_scaled[1:] - y_scaled[:-1]
    dist_tot = np.sum(np.sqrt(dx * dx + dy * dy))

    # number of interpolated points is proportional to the distance
    Nu = int(200 * dist_tot)
    u = np.arange(-1, Nu + 1) * 1. / (Nu - 1)

    # interpolate curve at sampled points
    k = min(3, len(x) - 1)
    res = interpolate.splprep([x_scaled, y_scaled], s=0, k=k)
    x_int, y_int = interpolate.splev(u, res[0]) 

    # we'll perturb perpendicular to the drawn line
    dx = x_int[2:] - x_int[:-2]
    dy = y_int[2:] - y_int[:-2]
    dist = np.sqrt(dx * dx + dy * dy)

    # create a filtered perturbation
    coeffs = mag * np.random.normal(0, 0.01, len(x_int) - 2)
    b = signal.firwin(f1, f2 * dist_tot, window=('kaiser', f3))
    response = signal.lfilter(b, 1, coeffs)

    x_int[1:-1] += response * dy / dist
    y_int[1:-1] += response * dx / dist

    # un-scale data
    x_int = x_int[1:-1] * (xlim[1] - xlim[0]) + xlim[0]
    y_int = y_int[1:-1] * (ylim[1] - ylim[0]) + ylim[0]
    
    return x_int, y_int


def handPlot(ax, mag=1.0,
            f1=50, f2=0.01, f3=15,
            bgcolor='w',
            xaxis_loc=None,
            yaxis_loc=None,
            xaxis_arrow='+',
            yaxis_arrow='+',
            ax_extend=0.1,
            expand_axes=True):
    """Make axis look hand-drawn

    Original author: Jake Vanderplas

    This is a script that will take any matplotlib line diagram, and convert it
    to an XKCD-style plot.  It will work for plots with line & text elements,
    including axes labels and titles (but not axes tick labels).

    The idea for this comes from work by Damon McDougall
      http://www.mail-archive.com/matplotlib-users@lists.sourceforge.net/msg25499.html

    This adjusts all lines, text, legends, and axes in the figure to look
    like xkcd plots.  Other plot elements are not modified.
    
    Parameters
    ----------
    ax : Axes instance
        the axes to be modified.
    mag : float
        the magnitude of the distortion
    f1, f2, f3 : int, float, int
        filtering parameters.  f1 gives the size of the window, f2 gives
        the high-frequency cutoff, f3 gives the size of the filter
    xaxis_loc, yaxis_log : float
        The locations to draw the x and y axes.  If not specified, they
        will be drawn from the bottom left of the plot
    xaxis_arrow, yaxis_arrow : str
        where to draw arrows on the x/y axes.  Options are '+', '-', '+-', or ''
    ax_extend : float
        How far (fractionally) to extend the drawn axes beyond the original
        axes limits
    expand_axes : bool
        if True, then expand axes to fill the figure (useful if there is only
        a single axes in the figure)
    """
    import numpy as np
    import pylab as pl
    import matplotlib.font_manager as fm

    # We need a special font for the code below.  It can be downloaded this way:
    import os
    import urllib2
    if not os.path.exists('Humor-Sans.ttf'):
        fhandle = urllib2.urlopen('http://antiyawn.com/uploads/Humor-Sans.ttf')
        open('Humor-Sans.ttf', 'wb').write(fhandle.read())
    
    # Get axes aspect
    ext = ax.get_window_extent().extents
    aspect = (ext[3] - ext[1]) / (ext[2] - ext[0])

    xlim = ax.get_xlim()
    ylim = ax.get_ylim()

    xspan = xlim[1] - xlim[0]
    yspan = ylim[1] - xlim[0]

    xax_lim = (xlim[0] - ax_extend * xspan,
               xlim[1] + ax_extend * xspan)
    yax_lim = (ylim[0] - ax_extend * yspan,
               ylim[1] + ax_extend * yspan)

    if xaxis_loc is None:
        xaxis_loc = ylim[0]

    if yaxis_loc is None:
        yaxis_loc = xlim[0]

    # Draw axes
    xaxis = pl.Line2D([xax_lim[0], xax_lim[1]], [xaxis_loc, xaxis_loc],
                      linestyle='-', color='k')
    yaxis = pl.Line2D([yaxis_loc, yaxis_loc], [yax_lim[0], yax_lim[1]],
                      linestyle='-', color='k')

    # Label axes3, 0.5, 'hello', fontsize=14)
    ax.text(xax_lim[1], xaxis_loc - 0.02 * yspan, ax.get_xlabel(),
            fontsize=14, ha='right', va='top', rotation=12)
    ax.text(yaxis_loc - 0.02 * xspan, yax_lim[1], ax.get_ylabel(),
            fontsize=14, ha='right', va='top', rotation=78)
    ax.set_xlabel('')
    ax.set_ylabel('')

    # Add title
    ax.text(0.5 * (xax_lim[1] + xax_lim[0]), yax_lim[1],
            ax.get_title(),
            ha='center', va='bottom', fontsize=16)
    ax.set_title('')

    Nlines = len(ax.lines)
    lines = [xaxis, yaxis] + [ax.lines.pop(0) for i in range(Nlines)]

    for line in lines:
        x, y = line.get_data()

        x_int, y_int = xkcd_line(x, y, xlim, ylim,
                                 mag, f1, f2, f3)

        # create foreground and background line
        lw = line.get_linewidth()
        line.set_linewidth(2 * lw)
        line.set_data(x_int, y_int)

        # don't add background line for axes
        if (line is not xaxis) and (line is not yaxis):
            line_bg = pl.Line2D(x_int, y_int, color=bgcolor,
                                linewidth=8 * lw)

            ax.add_line(line_bg)
        ax.add_line(line)

    # Draw arrow-heads at the end of axes lines
    arr1 = 0.03 * np.array([-1, 0, -1])
    arr2 = 0.02 * np.array([-1, 0, 1])

    arr1[::2] += np.random.normal(0, 0.005, 2)
    arr2[::2] += np.random.normal(0, 0.005, 2)

    x, y = xaxis.get_data()
    if '+' in str(xaxis_arrow):
        ax.plot(x[-1] + arr1 * xspan * aspect,
                y[-1] + arr2 * yspan,
                color='k', lw=2)
    if '-' in str(xaxis_arrow):
        ax.plot(x[0] - arr1 * xspan * aspect,
                y[0] - arr2 * yspan,
                color='k', lw=2)

    x, y = yaxis.get_data()
    if '+' in str(yaxis_arrow):
        ax.plot(x[-1] + arr2 * xspan * aspect,
                y[-1] + arr1 * yspan,
                color='k', lw=2)
    if '-' in str(yaxis_arrow):
        ax.plot(x[0] - arr2 * xspan * aspect,
                y[0] - arr1 * yspan,
                color='k', lw=2)

    # Change all the fonts to humor-sans.
    prop = fm.FontProperties(fname='Humor-Sans.ttf', size=16)
    for text in ax.texts:
        text.set_fontproperties(prop)
    
    # modify legend
    leg = ax.get_legend()
    if leg is not None:
        leg.set_frame_on(False)
        
        for child in leg.get_children():
            if isinstance(child, pl.Line2D):
                x, y = child.get_data()
                child.set_data(xkcd_line(x, y, mag=10, f1=100, f2=0.001))
                child.set_linewidth(2 * child.get_linewidth())
            if isinstance(child, pl.Text):
                child.set_fontproperties(prop)
    
    # Set the axis limits
    ax.set_xlim(xax_lim[0] - 0.1 * xspan,
                xax_lim[1] + 0.1 * xspan)
    ax.set_ylim(yax_lim[0] - 0.1 * yspan,
                yax_lim[1] + 0.1 * yspan)

    # adjust the axes
    ax.set_xticks([])
    ax.set_yticks([])      

    if expand_axes:
        ax.figure.set_facecolor(bgcolor)
        ax.set_axis_off()
        ax.set_position([0, 0, 1, 1])
    
    return ax
cdict = {'red': ((0.0, 0.1, 0.3),
                 (1.0, 0.6, 1.0)),
         'green': ((0.0, 0.0, 0.0),
                   (1.0, 0.6, 0.8)),
         'blue': ((0.0, 0.0, 0.0),
                  (1.0, 1, 0.8))}
INFOcmap = mpl.colors.LinearSegmentedColormap('my_colormap',cdict,256)

def _proba2rgb(p=0.99,cmap=INFOcmap):
    if p==0.0:
        r,g,b="FF","33","33"
    elif p==1.0:
        r,g,b="AA","FF","FF"
    else:
      (r,g,b,_)=cmap(p)
      r="%02x"%(r*256)
      g="%02x"%(g*256)
      b="%02x"%(b*256)

    return r,g,b

def _proba2bgcolor(p,cmap=INFOcmap):
    r,g,b=_proba2rgb(p,cmap)
    return "#"+r+g+b

def _proba2fgcolor(p,cmap=INFOcmap):
    if max([eval("0x"+s[0]) for s in _proba2rgb(p,cmap)])<=12:
        return "#FFFFFF"
    else:
        return "#000000"

def getBN(bn,size="4",vals=None,cmap=INFOcmap):
    """
    Shows a graphviz svg representation of the BN using size ("1" ,"2" , ...)
    vals is a dictionnary name:value of value in [0,1] for each node 
    (with special color for 0 and 1)
    """    
    graph=pydot.Dot(graph_type='digraph')
    for n in bn.names():
        if vals is None: 
          bgcol="#666666"
          fgcol="#FFFFFF"
        else:
          bgcol=_proba2bgcolor(vals[n],cmap)
          fgcol=_proba2fgcolor(vals[n],cmap)
        node=pydot.Node(n,style="filled",fillcolor=bgcol,fontcolor=fgcol)
        graph.add_node(node)
    for a in bn.arcs():
        edge=pydot.Edge(bn.variable(a[0]).name(),bn.variable(a[1]).name())
        graph.add_edge(edge)
    graph.set_size(size)
    return IPython.display.SVG(graph.create_svg())

def showBN(bn,size="4",vals=None,cmap=INFOcmap):
  gr=getBN(bn,size,vals,cmap)
  IPython.display.display(IPython.display.HTML("<div align='center'>"+gr.data+"</div>"))
  
  
def normalizeVals(vals,hilightExtrema=False):
    """
    normalisation if vals is not a proba (max>1)
    """
    ma=float(max(vals.values()))
    mi=float(min(vals.values()))
    if ma==mi:
      return None
    else:
      if not hilightExtrema:
        vmi=0.01
        vma=0.99
      else:
        vmi=0
        vma=1
    
      return {name:vmi+(val-mi)*(vma-vmi)/(ma-mi) for name,val in vals.iteritems()}
  
def showInference(bn,ie,size="4",cmap=INFOcmap):
  """
  Shows a bn annoted with results from inference : entropy and mutual informations
  """
  vals={bn.variable(n).name():ie.H(n) for n in bn.ids()}
  gr=getBN(bn,size,normalizeVals(vals,hilightExtrema=False),cmap)
  
  mi=min(vals.values())
  ma=max(vals.values())
  
  fig = mpl.figure.Figure(figsize=(8,3))
  canvas = fc(fig)
  ax1 = fig.add_axes([0.05, 0.80, 0.9, 0.15])
  norm = mpl.colors.Normalize(vmin=mi, vmax=ma)
  cb1 = mpl.colorbar.ColorbarBase(ax1, cmap=cmap,
                                   norm=norm,
                                   orientation='horizontal')
  cb1.set_label('Entropy')
  png=print_figure(canvas.figure,"png") # from IPython.core.pylabtools
  png_legend="<img style='vertical-align:middle' src='data:image/png;base64,%s'>"%encodestring(png).decode('ascii')
    
  IPython.display.display(IPython.display.HTML("<div align='center'>"+
                              gr.data+
                              "</div><div align='center'>"+
                              "<font color='"+_proba2bgcolor(0.01,cmap)+"'>"+str(mi)+"</font>"
                              +png_legend+
                              "<font color='"+_proba2bgcolor(0.99,cmap)+"'>"+str(ma)+"</font>"
                              "</div>"))
