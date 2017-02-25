% This command is used to format functions, variables and types in a different
% way. It is also used in the index.
\newcommand{\fu}[1]{\hbox{\tt #1}}

% \rw is used to format reserved words.
\newcommand{\rw}[1]{\hbox{\bf #1}}

% \type is used to set types, which are enclosed in braces.
% \type{int}, for example, generates {int} in bold
% letters.
\newcommand{\type}[1]{$\{\hbox{\bf #1}\}$}

% \poi is used to make a pointer type. \poi int, for example,
% yields *int.
\newcommand{\poi}{$*$}

% \arr is used to make an index for an array. \arr{10}int, for example,
% creates [10]int.
\newcommand{\arr}[1]{$[#1]$}

% \reference is used to indicate a reference type.
% \reference int, for example, yields &int.
\newcommand{\reference}{$\&$}

% The next commands are used to generate the four basic types of
% the SEL-language in order to reduce the typing work.
\newcommand{\tchar}{\type{char}}
\newcommand{\tint}{\type{int}}
\newcommand{\tfloat}{\type{float}}
\newcommand{\tvoid}{\type{void}}

% This command indicates a primary index.
\newcommand{\ii}[1]{\it #1}

% These here are used to typeset names of some programming languages
% in Small Caps letters.
\newcommand{\pascal}{\hbox{{\sc Pascal}}}
\newcommand{\modula}{\hbox{{\sc Modula-2}}}
\newcommand{\basic}{\hbox{{\sc Basic}}}
\newcommand{\dbase}{\hbox{d{\sc Base}}}
