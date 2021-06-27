This package depends on other packages in my github directory.

To get all code to successfully build follow the following steps:
 + Download charts into directory
   + git clone "https://github.com/colinw7/CQCharts" CQCharts
 + cd $ROOT/CQCharts/src
 + Check list of paths in CQCharts.pro (INCLUDEPATH variable)
   + for each path download the missing project into the same directory as you downloaded CQCharts
 + Check list of libraries in CQCharts/test/CQChartsTest.pro (unix:LIBS variable)
   + for each library download the missing project into the same directory as you downloaded CQCharts
 + build each downloaded library using its src and/or test directory
   + each library may have additional dependencies in its .pro or Makefile

Additional dependencies are standard UNIX packages:
 + readline
   + libreadline.so.7
 + png
   + libpng12.so.0
 + jpeg
   + libjpeg.so.8
 + tre
   + libtre.so.5
 + tcl
   + libtcl8.6.so
