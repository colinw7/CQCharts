#!/bin/csh -f

setenv CQMSG_HANDLER_WARN_ASSERT 1

foreach file (*.tcl)
  set log = log/$file:r.log

  ./charts.sh -pedantic -auto_exit $file >&! $log
end
