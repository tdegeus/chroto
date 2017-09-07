
********************
Notes for developers
********************

Getting started
===============

To start understanding the code one is advised to start from two sides:

1.  Study the structure and comments in ``mainwindow.h``.

2.  Study the structure and names in ``mainwindow.ui`` using Qt Creator. In Qt Creator a large portion of the function behind the buttons (the slots) can be studied.


Handling time
=============

Modules:

*   `date.h <https://howardhinnant.github.io/date/date.html>`_

.. code-block:: cpp

  #include "date.h"
  #include <chrono>
  #include <string>
  #include <iostream>

  int main()
  {
    std::string str = "2016:07:30 09:27:06";

    date::sys_seconds tm;

    std::istringstream iss{str};
    iss >> date::parse("%Y:%m:%d %H:%M:%S", tm);

    tm -= std::chrono::duration<int>(10);

    std::cout << date::format("%Y:%m:%d %H:%M:%S\n", tm);

    std::cout << date::format("%a\n", tm);

    return 0;
  }
