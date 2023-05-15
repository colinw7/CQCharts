set x {R}
set y {Theta}

set spiralA 1.0
set sprialN 5.0

for {set i 0} {$i < 360} {incr i 10} {
  lappend y $i
  lappend x [expr {$sprialN*pow($i, 1.0/$sprialN)}]
}

#set model [load_charts_model -csv data/polar.csv -first_line_header]
set model [load_charts_model -tcl [list $x $y] -first_line_header]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type scatter \
 -columns {{x 0} {y 1}}]

set_charts_property -plot $plot -name polar.enabled -value 1
set_charts_property -plot $plot -name connected.visible -value 1
