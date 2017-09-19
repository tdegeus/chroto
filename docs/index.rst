
******
chroto
******

.. note::

  This program is free to use. Any additions are very much appreciated, in terms of suggested functionality, code, documentation, testimonials, word of mouth advertisement, .... As always, the code comes with no guarantee. None of the developers can be held responsible for possible mistakes or misuse.

  Bug reports or feature requests can be filed on GitHub.

  (c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | `www.geus.me <http://www.geus.me>`_ | `github.com/tdegeus/chroto <http://github.com/tdegeus/chroto>`_

Contents
========

.. toctree::
   :maxdepth: 2

   develop.rst


for i in *.jpg; do exiv2 $i > test.log; if grep -q "Camera model    : Canon EOS DIGITAL REBEL" test.log; then mv $i cam0/; else mv $i cam1/; fi; done; rm test.log


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

