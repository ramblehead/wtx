#!/bin/bash

ulimit -c unlimited
./BootstrapToggle --config "../../approot/wt/wt_config.xml" \
                  --docroot "../../docroot" \
                  --docroot-static "js,css,fonts,img,resources,wtx" \
                  --approot "../../approot" \
                  --http-address 0.0.0.0 --http-port 8081 \
                  2>&1
