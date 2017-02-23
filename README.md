# wtx
wtx are RH's Extensions for [Wt, C++ Web Toolkit](https://www.webtoolkit.eu/). It is an experimental project aimed at looking for the right way to implement 3rd party Wt widgets...

Currently, only [Bootstrap Toggle](http://www.bootstraptoggle.com/) has been implemented.

To use this component, make sure that the content of ```server``` directory is in your includes search path, and the content of ```docroot``` is in your Wt server ```docroot```.

Then ```rh::wtx:: BootstrapToggle``` can be used as was a stock Wt toggle:

```C
#include wtx/BootstrapToggle.hpp
```
