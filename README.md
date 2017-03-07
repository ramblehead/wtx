#wtx: Wt Extensions

_wtx_ is an experimental project aimed at looking for the right way to implement 3rd party extensions and widgets for [Wt, C++ Web Toolkit](https://www.webtoolkit.eu/).

Currently, only [Bootstrap Toggle](http://www.bootstraptoggle.com/) is implemented.

To use this component, make sure that the content of ```server``` directory is in your includes search path, and the content of ```docroot``` is in your Wt server ```docroot```.

Then ```rh::wtx:: BootstrapToggle``` can be used as was a stock Wt toggle:

```C
#include wtx/BootstrapToggle.hpp
```
