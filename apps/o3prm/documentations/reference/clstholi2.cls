% -*- coding: UTF-8 -*- 

%%% ====================================================================
%%%  @LaTeX-class-file{
%%% filename    = "clstholi2.cls",
%%% version     = "1.2",
%%% date        = "28 Septembre 2010",
%%% writtenby   = "L. Torti from a file of O. Spanjaard (who work from a file
%%%                of J. Zaitseff)",
%%% email       = "lionel.torti@lip6.fr",
%%% keywords    = "LaTeX, thesis",
%%% docstring   = "This file provides a thesis class for writing thesis."
%%% }
%%% ====================================================================

% This file provides a thesis class for the "mémoire de présoutenance" of
% Olivier (and a lot more of people in fact: Paul, Lucie, JP, Gildas, Thomas
% and % Lionel). It is meant to be used with LaTeX-2e, and exists because of
% the archaic standards required by universities (surprised?) all over the
% world with regards to thesis styles.

% Last changes included switching to UTF-8 and using the Times font by
% default.

% As with all class files, this class is meant to be used as the exclusive
% LaTeX-2e class for the entire document.  It may be introduced in the
% document via:
%
%       \documentclass{clstholi2}

% As always, options may be placed before the {clstholi}.  In this case,
% the following options are supported:
%
%       singlespacing   - Single-space the document (ie, do not apply the
%                         thesis requirements).
%       doublespacing   - Double-space the document (actually, space it
%                         1.21 times the baseline).
%       draft           - Prints the word "DRAFT" across the front page,
%                         as well as on each page (in the footer).
%       final           - Opposite of draft: do not print "DRAFT".

% All of the options for the Book class are also supported, namely,
%       10pt, 11pt, 12pt             (11pt is used by default)
%       letterpaper, legalpaper,
%       executivepaper, a4paper,
%       a5paper, b5paper             (a4paper used by default)
%       landscape                    (not used by default)
%       oneside, twoside             (oneside used by default)
%       openright, openany           (openany used by default)
%       onecolumn, twocolumn         (onecolumn used by default)
%       openbib                      (not used by default)
%       leqn, fleqn                  (not used by default)
%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{clstholi2}%
    % [2003/07/10 v1 Document de these de Olivier]
\RequirePackage{fancyhdr}

\usepackage[T1]{fontenc}
\usepackage[utf8]{inputenc}
\usepackage{vmargin}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   Changeable Parameters   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Change these for customisation.  To change them in a LaTeX document,
% simply use \thesisuni{xxx} and \thesisschool{xxx}.

\gdef\@thesisuni{\sc Université Pierre et Marie Curie (PARIS VI)\\ ufr ingénierie}
\gdef\@thesisschool{\'Ecole Doctorale Informatique, Télécommunications et \'Electronique}

% Customise for foreign languages.

\def\supervisorname{Directeur de thèse~: Christophe GONZALES}
\def\assessorname{JURY\\ [1ex]}
%\def\abstractname{Abstr}

%%%%%%%%%%%%%%%%%%%%%
%   Class options   %
%%%%%%%%%%%%%%%%%%%%%

\newif\if@dspacing{}
\newif\if@thdraft{}
\newcommand\droptnm{final}

\DeclareOption{singlespacing}{\@dspacingfalse}
\DeclareOption{doublespacing}{\@dspacingtrue}
\DeclareOption{draft}{\@thdrafttrue\renewcommand\droptnm{draft}}
\DeclareOption{final}{\@thdraftfalse\renewcommand\droptnm{final}}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{book}}
\ExecuteOptions{doublespacing,final} \ProcessOptions
\LoadClass[a4paper,11pt,oneside,openany,\droptnm]{book}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   Draft / final options   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\if@thdraft%
    \RequirePackage[first,light]{draftcopy}%
\fi

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   Double and single spacing   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\newcommand{\ssp}{%
    \def\baselinestretch{1}%
    \large\normalsize\selectfont}
\newcommand{\dsp}{%
    \def\baselinestretch{1.21}%
    \large\normalsize\selectfont}

% Define conditional:

\if@dspacing%
    \newcommand\s@dsp{\dsp}%
\else%
    \newcommand\s@dsp{\ssp}%
\fi

% Conditional Double-spacing environment

\def\spacing{%
    \begingroup%
    \s@dsp}%
\def\endspacing{%
    \par \vskip \parskip \vskip \baselineskip%
    \endgroup%
    \vskip -\parskip%
    \vskip -\baselineskip}

% Always single-space

\def\sspacing{%
    \begingroup%
    \ssp}%
\def\endsspacing{%
    \par \vskip \parskip \vskip \baselineskip%
    \endgroup%
    \vskip -\parskip%
    \vskip -\baselineskip}

% Execute the following ONLY if double-spacing was selected.

\if@dspacing
    % The following are from "book.cls":
    \renewcommand{\mainmatter}{%
    \cleardoublepage\@mainmattertrue\pagenumbering{arabic}\s@dsp}
    \renewcommand{\backmatter}{%
    \if@openright\cleardoublepage\else\clearpage\fi%
    \@mainmatterfalse\ssp}
    \renewenvironment{figure}[1][tbp]{\@float{figure}[#1]\ssp}{\end@float\s@dsp}
    \renewenvironment{figure*}[1][tbp]{\@dblfloat{figure}[#1]\ssp}{\end@dblfloat\s@dsp}
    \renewenvironment{table}[1][tbp]{\@float{table}[#1]\ssp}{\end@float\s@dsp}
    \renewenvironment{table*}[1][tbp]{\@dblfloat{table}[#1]\ssp}{\end@dblfloat\s@dsp}

    % The following are from "latex.ltx". BUG: This is NOT single-spaced.

    \long\def\@footnotetext#1{\insert\footins{%
    \ssp%                  % Should be single-spaced, but...
    \reset@font\footnotesize
    \interlinepenalty\interfootnotelinepenalty
    \splittopskip\footnotesep
    \splitmaxdepth \dp\strutbox \floatingpenalty \@MM
    \hsize\columnwidth \@parboxrestore
    \protected@edef\@currentlabel{%
        \csname p@footnote\endcsname\@thefnmark
    }%
    \color@begingroup
        \@makefntext{%
        \rule\z@\footnotesep\ignorespaces#1\@finalstrut\strutbox}
    \color@endgroup}}
\fi

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   Miscellaneous Commands   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\DeclareFontShape{OT1}{cmr}{bx}{sl}{<-> cmbxsl10}{}
\newcommand{\HUGE}{%
    \fontencoding{OT1}\fontfamily{cmr}%
    \fontseries{bx}\fontshape{sl}%
    \fontsize{14}{20}\selectfont}

\newcommand{\thesisuni}[1]{\gdef\@thesisuni{#1}}
\newcommand{\thesisschool}[1]{\gdef\@thesisschool{#1}}
\newcommand{\thesistitle}[1]{\gdef\@thesistitle{#1}}
\newcommand{\thesisauthor}[1]{\gdef\@thesisauthor{#1}}
\newcommand{\thesisdegree}[1]{\gdef\@thesisdegree{#1}}
\newcommand{\thesisdate}[1]{\gdef\@thesisdate{#1}}
\newcommand{\thesissupervisor}[1]{\gdef\@thesissupervisor{#1}}
\newcommand{\thesisassessor}[1]{\gdef\@thesisassessor{#1}}

\newcommand{\@thesistitle}{\@latex@error{No \noexpand\thesistitle given}\@ehc}
\newcommand{\@thesisauthor}{\@latex@error{No \noexpand\thesisauthor given}\@ehc}
\newcommand{\@thesisdegree}{\@latex@error{No \noexpand\thesisdegree given}\@ehc}
\newcommand{\@thesisdate}{\@latex@error{No \noexpand\thesisdate given}\@ehc}
\newcommand{\@thesissupervisor}{\@latex@error{No \noexpand\thesissupervisor given}\@ehc}
\newcommand{\@thesisassessor}{\@latex@error{No \noexpand\thesisassessor given}\@ehc}

\renewcommand{\title}[1]{\@latex@warning{\noexpand\title is not used with this class}}
\renewcommand{\author}[1]{\@latex@warning{\noexpand\author is not used with this class}}
\renewcommand{\date}[1]{\@latex@warning{\noexpand\date is not used with this class}}


%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   Thesis front matter   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Title page

\renewcommand{\maketitle}%
    {\begin{titlepage}%
    \begin{figure}[h!]
    \centering
    \subfloat{\includegraphics[clip,width=0.30\textwidth]{edite.pdf}}
    \hfill
    \subfloat{ \includegraphics[clip,width=5.2cm]{upmc.pdf}}
    \end{figure}
    \vspace{-1cm}
    %\begin{center}
    %{\Large \scshape \@thesisuni \par}%
    %\end{center}
    \begin{center}%
    {\HUGE \bf THESE}%
    \vskip 1em %
    Pour l'obtention du titre de\\
    DOCTEUR EN INFORMATIQUE\\
    \vskip .7em %
    {\it spécialité~: aide à la décision}
    \vskip 2em%
    {\HUGE \@thesistitle \par}%
    \vskip 2em%
    \begin{tabular}{rl}
    {\large \textsl{Candidat~:}} & {\large Lionel TORTI}\\
    %{\large \textsl{\supervisorname}} & {\large \@thesissupervisor}
    \end{tabular}
    %{\large \@thesisdegree \par}%
    \vskip 2em%
    %{\large \textsl{Soutenance:}\ \@thesisdate \par}%
    %\vskip 7em%
    {\large \textsl{\assessorname}}
    \vskip .7em%
    {\@thesisassessor \par}%
    % \vskip 2em%
    % Présentée et soutenue publiquement le 21/11/2011
    \end{center}\par
    %\vskip 30\p@
    %\vfil\null
    \end{titlepage}%
    \setcounter{footnote}{0}%
    \let\maketitle\relax}

% Abstract environment (not defined by "book.cls")

\newenvironment{abstract}{%
    \titlepage
    \null\vfil
    \begin{center}%
    {\Large \bfseries \abstractname}
    \@endparpenalty\@M
    \end{center}\spacing}%
    {\endspacing\par\vfil\null\endtitlepage}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   General Page Parameters   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Marges.
\setpapersize{A4}
%\setmargins{0.13\paperwidth}{0.07\paperheight}{0.74\paperwidth}{0.80\paperheight}{0.01\paperheight}{0.02\paperheight}{0cm}{0.07\paperheight}
\setmargins{0.15\paperwidth}{0.10\paperheight}{0.70\paperwidth}{0.75\paperheight}%
{0.01\paperheight}{0.02\paperheight}{0cm}{0.07\paperheight}
%\setlength{\oddsidemargin}{29pt}%
%\setlength{\evensidemargin}{80pt}%
%\setlength{\marginparwidth}{113pt}%
%\setlength{\marginparsep}{10pt}%
%\setlength{\marginparpush}{5pt}%
%\setlength{\topmargin}{21pt}%
%\setlength{\headheight}{12pt}%
%\setlength{\headsep}{25pt}%
%\setlength{\footskip}{30pt}%
%\setlength{\textheight}{40\baselineskip}%
%\setlength{\textwidth}{360pt}%
%\setlength{\columnsep}{10pt}%
%\setlength{\columnseprule}{0pt}%

% Page headings and footers.

\pagestyle{fancyplain}
\renewcommand{\chaptermark}[1]{\markboth{\thechapter. #1}{\thechapter. #1}}
\renewcommand{\sectionmark}[1]{}
\lhead[\fancyplain{}{\bfseries\thepage}]{\fancyplain{}{\bfseries\rightmark}}
\rhead[\fancyplain{}{\bfseries\leftmark}]{\fancyplain{}{\bfseries\thepage}}
\cfoot{\fancyplain{\textrm{\thepage}}{}}

\if@thdraft
    \rfoot[\textbf{Draft:}~\today]{}
    \lfoot[]{\textbf{Draft:}~\today}
\fi
