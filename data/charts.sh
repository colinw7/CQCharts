#!/bin/csh -f

set args     = ()
set opts     = ()
set debug    = 0
set valgrind = 0
set csv_args = ()

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
    setenv HRTIMER_ACTIVE 1
    shift
  else if ("$1" == "-pixmap") then
    setenv CQCHARTS_LAYER_PIXMAP 1
    shift
  else if ("$1" == "-csv") then
    set csv_args = ($csv_args $1)
    shift

    if ($#argv > 0) then
      set csv_args = ($csv_args $1)
      shift
    endif
  else if ("$1" == "-first_line_header") then
    set csv_args = ($csv_args $1)
    shift
  else if ("$1" == "-first_column_header") then
    set csv_args = ($csv_args $1)
    shift
  else if ("$1" == "-h" || "$1" == "-help") then
    echo "charts.sh [-dbx|-valgrind|-loop|-timer|-pixmap] <script>"
    exit 1
  else
    set args = ($args $1)
    shift
  endif
end

set exec_args = ()

if ($#args > 0) then
  set exec_args = (-exec $args)
endif

setenv QT_AUTO_SCREEN_SCALE_FACTOR 0
setenv QT_SCALE_FACTOR 0

if      ($debug == 1) then
  echo "Dbx CQChartsTest $exec_args $csv_args $opts"

  Dbx CQChartsTest $exec_args $csv_args $opts
else if ($valgrind == 1) then
  echo "Valgrind CQChartsTest $exec_args $csv_args $opts"

  Valgrind CQChartsTest $exec_args $csv_args $opts
else
  echo "CQChartsTest $exec_args $csv_args $opts"

  CQChartsTest $exec_args $csv_args $opts
endif

exit 0
