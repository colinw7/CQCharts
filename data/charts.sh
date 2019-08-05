#!/bin/csh -f

set args      = ()
set opts      = ()
set dbx       = 0
set valgrind  = 0
set helgrind  = 0
set git       = 0
set save      = 0
set csv_args  = ()
set tsv_args  = ()
set data_args = ()
set nograb    = ""
set modal     = 1

while ($#argv > 0)
  if      ("$1" == "-dbx") then
    set dbx = 1
    set nograb = "-nograb"
    shift
  else if ("$1" == "-valgrind") then
    set valgrind = 1
    shift
  else if ("$1" == "-helgrind") then
    set helgrind = 1
    shift
  else if ("$1" == "-git") then
    set git = 1
    shift
  else if ("$1" == "-save") then
    set save = 1
    shift
  else if ("$1" == "-loop") then
    set opts = ($opts -loop)
    shift
  else if ("$1" == "-debug") then
    setenv CQ_CHARTS_DEBUG 1
    shift
  else if ("$1" == "-timer" || "$1" == "-perf_debug") then
    setenv HRTIMER_ACTIVE 1
    setenv CQ_PERF_MONITOR_DEBUG 1
    setenv CQ_PERF_MONITOR_DEBUG_PATTERN "*"
    setenv CQ_CHARTS_DEBUG_UPDATE 1
    shift
  else if ("$1" == "-perf") then
    setenv CQ_PERF_MONITOR_ENABLED 1
    shift
  else if ("$1" == "-quad_tree") then
    setenv CQ_CHARTS_DEBUG_QUAD_TREE 1
    shift
  else if ("$1" == "-record") then
    setenv CQ_CHARTS_OBJ_TREE_WAIT 1
    setenv CQ_CHARTS_PLOT_UPDATE_TIMEOUT -1
    set opts = ($opts -record)
    shift
  else if ("$1" == "-pixmap") then
    setenv CQ_CHARTS_LAYER_PIXMAP 1
    shift
  else if ("$1" == "-csv") then
    set csv_args = ($csv_args $1)
    shift

    if ($#argv > 0) then
      set csv_args = ($csv_args $1)
      shift
    endif
  else if ("$1" == "-tsv") then
    set tsv_args = ($tsv_args $1)
    shift

    if ($#argv > 0) then
      set tsv_args = ($tsv_args $1)
      shift
    endif
  else if ("$1" == "-first_line_header") then
    set data_args = ($data_args $1)
    shift
  else if ("$1" == "-first_column_header") then
    set data_args = ($data_args $1)
    shift
  else if ("$1" == "-comment_header") then
    set data_args = ($data_args $1)
    shift
  else if ("$1" == "-modal") then
    set modal = 1
    shift
  else if ("$1" == "-no_modal") then
    set modal = 0
    shift
  else if ("$1" == "-meta_edit") then
    setenv CQ_CHARTS_META_EDIT 1
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

setenv CQ_CHARTS_MODAL_DLG $modal

set exe = CQChartsTest

if ($git == 1) then
  set exe = ~/git/CQCharts/bin/CQChartsTest
endif

if ($save == 1) then
  set exe = /home/colinw/bin/CQChartsSaveTest
endif

if      ($dbx == 1) then
  echo "Dbx $exe $exec_args $csv_args $tsv_args $data_args $opts $nograb"

  Dbx $exe $exec_args $csv_args $tsv_args $data_args $opts $nograb
else if ($valgrind == 1) then
  echo "Valgrind $exe $exec_args $csv_args $tsv_args $data_args $opts"

  Valgrind $exe $exec_args $csv_args $tsv_args $data_args $opts
else if ($helgrind == 1) then
  echo "Helgrind $exe $exec_args $csv_args $tsv_args $data_args $opts"

  Helgrind $exe $exec_args $csv_args $tsv_args $data_args $opts
else
  echo "$exe $exec_args $csv_args $tsv_args $data_args $opts"

  $exe $exec_args $csv_args $tsv_args $data_args $opts
endif

exit 0
