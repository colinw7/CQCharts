#!/bin/csh -f

set args  = ()
set opts  = ()
set debug = 0

while ($#argv > 0)
  if      ("$1" == "-dbx") then
    set debug = 1
    shift
  else if ("$1" == "-loop") then
    set opts = ($opts -loop)
    shift
  else
    set args = ($args $1)
    shift
  endif
end

if ($debug == 1) then
  Dbx CQChartsTest -tcl -exec $args $opts
else
  CQChartsTest -tcl -exec $args $opts
endif

exit 0
