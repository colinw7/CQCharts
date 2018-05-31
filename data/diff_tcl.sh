#!/bin/csh -f

foreach file (*.cl)
  set file1 = $file:r.tcl

  echo "--- $file1 ---"

  if (! -e $file1) then
    echo "  Missing file $file1"
  else
    #diff $file $file1 | sed 's/^/  /'
  endif
end

exit 0
