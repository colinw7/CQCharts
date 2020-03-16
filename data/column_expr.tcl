set x {X}
set y {Y}

for {set i 0} {$i < 100} {incr i} {
  lappend x $i
  lappend y [expr {cos($i/10.0)}]
}

set modelId [load_charts_model -tcl [list $x $y] -first_line_header]

set plotId [create_charts_plot -type xy \
  -columns {{x X} {y {{#COS (cos($row/10.0))}}}} -title "X/Y"]
