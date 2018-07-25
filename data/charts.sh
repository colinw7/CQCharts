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
  if ($#args > 0) then
    echo "Dbx CQChartsTest -exec $args $opts"

    Dbx CQChartsTest -exec $args $opts
  else
    echo "Dbx CQChartsTest $opts"

    Dbx CQChartsTest $opts
  endif
else if ($valgrind == 1) then
  if ($#args > 0) then
    echo "Valgrind CQChartsTest -exec $args $opts"

    Valgrind CQChartsTest -exec $args $opts
  else
    echo "Valgrind CQChartsTest $opts"

    Valgrind CQChartsTest $opts
  endif
else
  if ($#args > 0) then
    echo "CQChartsTest -exec $args $opts"

    CQChartsTest -exec $args $opts
  else
    echo "CQChartsTest $opts"

    CQChartsTest $opts
  endif
endif

exit 0
