\documentclass{article}
\usepackage{geometry}
\usepackage{hyperref}
\usepackage{graphicx}
\usepackage{listings}
\usepackage{xcolor}
\usepackage{fontspec}
\usepackage{titlesec}

\geometry{a4paper, margin=1in}
\setmainfont{Times New Roman}
\titleformat{\section}{\Large\bfseries}{\thesection}{1em}{}
\titleformat{\subsection}{\large\bfseries}{\thesubsection}{1em}{}
\titleformat{\subsubsection}{\bfseries}{\thesubsubsection}{1em}{}

\definecolor{codegreen}{rgb}{0,0.6,0}
\definecolor{codegray}{rgb}{0.5,0.5,0.5}
\definecolor{codepurple}{rgb}{0.58,0,0.82}
\definecolor{backcolour}{rgb}{0.95,0.95,0.92}

\lstdefinestyle{mystyle}{
    backgroundcolor=\color{backcolour},
    commentstyle=\color{codegreen},
    keywordstyle=\color{magenta},
    numberstyle=\tiny\color{codegray},
    stringstyle=\color{codepurple},
    basicstyle=\ttfamily\footnotesize,
    breakatwhitespace=false,
    breaklines=true,
    captionpos=b,
    keepspaces=true,
    numbers=left,
    numbersep=5pt,
    showspaces=false,
    showstringspaces=false,
    showtabs=false,
    tabsize=2
}

\lstset{style=mystyle}

\begin{document}

\title{\textbf{OpenGL Project}}
\author{Your Name}
\date{\today}
\maketitle

\section*{Overview}
This project is an interactive 3D scene built using OpenGL. It features various lighting effects, animations, and environmental controls. Below is a detailed description of the keyboard controls and features available in the project.

\section*{Keyboard Controls}
\begin{itemize}
    \item \textbf{W}: Move the camera forward.
    \item \textbf{S}: Move the camera backward.
    \item \textbf{A}: Move the camera to the left.
    \item \textbf{D}: Move the camera to the right.
    \item \textbf{M}: Toggle the shadow map visualization.
    \item \textbf{N}: Activate the scarecrow animation, causing it to rotate around itself.
    \item \textbf{P}: Enable the point light and switch to a night environment.
    \item \textbf{Q}: Toggle the directional light on and off.
    \item \textbf{L}: Move the directional light forward.
    \item \textbf{J}: Move the directional light backward.
    \item \textbf{R}: Start the snow effect.
    \item \textbf{T}: Begin an automated tour of the scene.
\end{itemize}

\section*{Features}
\subsection*{Shadow Map Visualization}
Press \textbf{M} to toggle the shadow map visualization. This allows you to see how shadows are being rendered in the scene.

\subsection*{Scarecrow Animation}
Press \textbf{N} to activate the scarecrow animation. The scarecrow will rotate around itself, adding a dynamic element to the scene.

\subsection*{Point Light and Night Environment}
Press \textbf{P} to enable the point light and switch to a night environment. This changes the lighting and ambiance of the scene to simulate nighttime.

\subsection*{Directional Light Control}
Press \textbf{Q} to toggle the directional light on and off. Use \textbf{L} and \textbf{J} to move the directional light forward and backward, respectively.

\subsection*{Snow Effect}
Press \textbf{R} to start the snow effect. This adds a layer of falling snow to the scene, enhancing the winter atmosphere.

\subsection*{Automated Scene Tour}
Press \textbf{T} to begin an automated tour of the scene. The camera will move through the scene, giving you a guided view of all the elements.

\section*{Getting Started}
To run this project, ensure you have the necessary dependencies installed, including OpenGL and GLUT. Clone the repository and compile the source code using your preferred C++ compiler.

\begin{lstlisting}[language=bash]
git clone https://github.com/yourusername/OpenGLProject.git
cd OpenGLProject
make
./OpenGLProject
\end{lstlisting}

\section*{Contributing}
Contributions are welcome! Please fork the repository and submit a pull request with your changes. Ensure your code adheres to the project's coding standards and includes appropriate documentation.

\section*{License}
This project is licensed under the MIT License. See the \href{LICENSE}{LICENSE} file for more details.

\section*{Acknowledgments}
\begin{itemize}
    \item Thanks to the OpenGL community for their extensive documentation and support.
    \
