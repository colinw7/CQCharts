#!/bin/csh -f

# Instructions
#  . make empty build directory
#  . cp build.sh to empty build directory
#  . Run ./build.sh

set cdir = `pwd`

if (! -e CQCharts) then
  git clone https://github.com/colinw7/CQCharts.git CQCharts
endif

set inc = `cat CQCharts/src/CQCharts.pro | sed ':x; /\\$/ { N; s/\\\n//; tx }' | grep INCLUDEPATH | sed 's/INCLUDEPATH += //'`

set dirs = ()

foreach v ($inc)
  set n = `echo $v | grep '..\/..\/' | wc -l`

  if ($n != 1) then
    continue
  endif

  set v1 = `echo $v | sed 's@../../\(.*\)/include@\1@'`

  set dirs = ($dirs $v1)

  if (! -e $v1) then
    echo "--- Clone $v1 ---"

    git clone https://github.com/colinw7/${v1}.git $v1
  endif
end

foreach dir ($dirs)
  echo "--- Build $dir ---"

  if (-e $cdir/$dir/src) then
    cd $cdir/$dir/src

    if (-e "*.pro") then
      qmake
    endif

    make
  endif
end

echo "--- Build CQCharts (src) ---"

cd $cdir/CQCharts/src

qmake

make

echo "--- Build CQCharts (test) ---"

cd $cdir/CQCharts/test

qmake

make
