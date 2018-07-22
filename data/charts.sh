#!/bin/csh -f

set args     = ()
set opts     = ()
set debug    = 0
set timer    = 0
set valgrind = 0

while ($#argv > 0)
  if      ("$1" == "-dbx") then
    set debug = 1
    shift
  else if ("$1" == "-valgrind") then
    set valgrind = 1
    shift
  else if ("$1" == "-loop") then
    set opts = ($opts -loop)
    shift
  else if ("$1" == "-timer") then
    set timer = 1
    shift
  else if ("$1" == "-h" || "$1" == "-help") then
    echo "charts.sh -dbx|-valgrind|-loop|-timer <script>"
    exit 1
  else
    set args = ($args $1)
    shift
  endif
end

setenv QT_AUTO_SCREEN_SCALE_FACTOR 0
setenv QT_SCALE_FACTOR 0

if ($timer == 1) then
  setenv HRTIMER_ACTIVE 1
endif

if      ($debug == 1) then
  Dbx CQChartsTest -exec $args $opts
else if ($valgrind == 1) then
  Valgrind CQChartsTest -exec $args $opts
else
  CQChartsTest -exec $args $opts
endif

exit 0
