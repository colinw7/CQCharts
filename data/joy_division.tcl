# https://observablehq.com/@saneef/004-joy-division

# TODO: perlin rand

proc tcl::mathfunc::max { a b } {
  if {$a > $b} {
    return $a
  } else {
    return $b
  }
}

proc tcl::mathfunc::min { a b } {
  if {$a < $b} {
    return $a
  } else {
    return $b
  }
}

set size 400
set step 10

set rows [expr {39*39}]

set model [create_charts_data_model -rows $rows -columns 3]

set_charts_data -model $model -column 0 -header -name value -value Group
set_charts_data -model $model -column 1 -header -name value -value X
set_charts_data -model $model -column 2 -header -name value -value Y

set row  0
set line 0

for {set i $step} {$i <= $size - $step} {incr i $step} {
  for {set j $step} {$j <= $size - $step} {incr j $step} {
    set distanceToCenter [expr {abs($j - $size/2)}]

    set variance [expr {max($size/2 - 50 - $distanceToCenter, 0)}]

    set random [expr {rand()*$variance/2*-1}]

    set x $j
    set y [expr {$i + $random}]

    set y [expr {$size - $y}]

    set_charts_data -model $model -row $row -column 0 -name value -value $line
    set_charts_data -model $model -row $row -column 1 -name value -value $x
    set_charts_data -model $model -row $row -column 2 -name value -value $y

    incr row
  }

  incr line
}

set plot [create_charts_plot -type xy -columns {{x 1} {y 2} {group 0}} \
 -xmin 0 -ymin 0 -xmax 400 -ymax 455]

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name theme.dark -value 1

set_charts_property -plot $plot -name key.visible -value 0

set_charts_property -plot $plot -name lines.stroke.color -value white
set_charts_property -plot $plot -name lines.rounded      -value 1

set_charts_property -plot $plot -name fillUnder.visible    -value 1
set_charts_property -plot $plot -name fillUnder.fill.color -value black
set_charts_property -plot $plot -name fillUnder.fill.alpha -value 1

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0

set_charts_property -plot $plot -name margins.outer.left   -value 0
set_charts_property -plot $plot -name margins.outer.right  -value 0
set_charts_property -plot $plot -name margins.outer.top    -value 0
set_charts_property -plot $plot -name margins.outer.bottom -value 0
