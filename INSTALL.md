

Linux 
-----

  1. Clone the repository using:
     
         $ git clone https://github.com/tdegeus/chroto.git --recursive;
     
     Note that the option ``--recursive`` is required to clone the included git
     submodules as well.

  2. Generate the approriate makefile using

         $ cd chroto/src/;
         $ qmake chroto.pro;

  3. Compile the project using

         $ make;

