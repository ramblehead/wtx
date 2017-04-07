# wtx: Wt Extensions

_wtx_ is an experimental project looking for the right way of implementing [Wt, C++ Web Toolkit](https://www.webtoolkit.eu/) interface classes for 3rd party JavaScript widgets.

_wtx_ aims to simpify Wt and JavaScript widgets implementation by reusing original or minimally altered JavaScript code.

Currently, only [Bootstrap Toggle](http://www.bootstraptoggle.com/) is implemented.

To use this component, make sure that the content of ```server``` directory is in your includes search path, and the content of ```docroot``` is in your Wt server ```docroot```.

Then ```rh::wtx:: BootstrapToggle``` can be used as it was a stock ```Wt::WCheckBox```:

```C
#include wtx/BootstrapToggle.hpp
```
