% These are for the index.
\def\seeterm{siehe}
\newcommand{\fuindex}[1]{\index{#1@\fu{#1}}}
\newcommand{\fuindexii}[1]{\index{#1@\fu{#1}|ii}}
\newcommand{\fuindexuu}[1]{\index{#1@\fu{#1}|uu}}
\newcommand{\rwindex}[1]{\index{#1@\rw{#1}}}
\newcommand{\rwindexii}[1]{\index{#1@\rw{#1}|ii}}
\newcommand{\rwindexuu}[1]{\index{#1@\rw{#1}|uu}}
\newcommand{\typeindex}[1]{\index{#1@\protect\type{#1}}}
\newcommand{\typeindexii}[1]{\index{#1@\protect\type{#1}|ii}}

\newcommand{\ind}[1]{#1\index{#1}}
\newcommand{\indi}[1]{#1\index{#1|ii}}
\newcommand{\indu}[1]{#1\index{#1|uu}}

% This defines the inch font which is used for the title page.
\newfont{\inch}{cminch scaled 1000}

% This command is used to format functions, variables and types in a different
% way. It is also used in the index.                             
\newcommand{\fu}[1]{\hbox{\tt #1}}
\newcommand{\fui}[1]{\fu{#1}\fuindex{#1}}
\newcommand{\fuii}[1]{\fu{#1}\fuindexii{#1}}
\newcommand{\fuiu}[1]{\fu{#1}\fuindexuu{#1}}

% \rw is used to format reserved words.
\newcommand{\rw}[1]{\hbox{\bf #1}}
\newcommand{\rwi}[1]{\rw{#1}\rwindex{#1}}
\newcommand{\rwii}[1]{\rw{#1}\rwindexii{#1}}
\newcommand{\rwiu}[1]{\rw{#1}\rwindexuu{#1}}

% \type is used to set types, which are enclosed in square
% brackets. \type{int}, for example, generated [int] in bold
% letters.
\newcommand{\type}[1]{$\{\hbox{\bf #1}\}$}
                             
% \poi is used to make a pointer type. \poi int, for example,
% yields *int.
\newcommand{\poi}{$*$}

% \arr is used to make an index for an array. \arr{10}int, for example,
% creates [10]int.
\newcommand{\arr}[1]{$[#1]$}    

% The next commands are used to generate the four basic types of
% the SEL-language in order to reduce the typing work.
\newcommand{\tchar}{\type{char}}
\newcommand{\tint}{\type{int}}
\newcommand{\tfloat}{\type{float}}
\newcommand{\tvoid}{\type{void}}

% The environment operator is used to describe the operators with
% the types of their operands and the resulting type.
\newenvironment{operator}{\medskip\begin{tabular}%
{r@{$\;\Longrightarrow\;$}l}}{\end{tabular}\medskip}

% This command indicates a primary index...
\newcommand{\ii}[1]{{\it #1}}
% ...and this indicates an example.
\newcommand{\uu}[1]{\underline{#1}}

% \bullettitle is used for some titles in the function-reference. It makes
% a title with bold letters and a bullet on the left side.
% \newcommand{\bullettitle}[1]{\vspace{0.3cm}\noindent{{\bf #1}%
% \marginpar{\hfill$\bullet$}}\par\nobreak\noindent}
\newcommand{\bullettitle}[1]{\medskip%
\noindent{{$\bullet$\hspace{0.5ex}{\bf #1}}}\par\nobreak\noindent}
\newcommand{\bullettitlewop}[1]{\medskip%
\noindent{{$\bullet$\hspace{0.5ex}{\bf #1}}}}
                                                          
% function creates a title for the function-reference.
\newenvironment{function}[1]{\bigskip\fuindexii{#1}%
\noindent{\huge\sf #1}\begin{nopagebreak}}{\end{nopagebreak}}
                               
% \syntax makes a bullet title and formats the given parameter as verbatim.
% This command is used in the function-reference.
\newcommand{\syntax}{\bullettitlewop{Syntax}\begingroup\parindent0em\obeylines\originalTeX\tt}
                                                     
% the parameters environment is used to describe the paramaters for a function
% in the function reference. It is defined as a tabular environment with
% three rows (name, type, description) with a fixed width.
\newcommand{\parameters}{\bullettitle{Parameter}%
\settabs\+\hspace{2cm}&\hspace{2cm}&\hspace{7.3cm}&\cr}

% We need this command in the parameters-environment to format the first two
% rows with an italic font.
\newcommand{\param}[2]{\+\fu{#1} & #2 &}
                           
% \usage simply creates a bullet title and is used in the function-reference.
\newcommand{\usage}{\bullettitle{Beschreibung}}

% \example creates a bulletin title and is used both in the
% function- and in the grammar-reference
\newcommand{\example}{\bullettitle{Beispiel}}

% \fseealso simply creates a bullet title and is used in the function-reference.
\newcommand{\fseealso}{\bullettitle{siehe auch}}
