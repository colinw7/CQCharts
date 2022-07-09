set x {X}
set y {Y}
set c {Color}

for {set i 0} {$i < 100} {incr i} {
  set yi [expr {cos($i/10.0)}]

  lappend x $i
  lappend y $yi

  if {$yi < 0.0} {
    lappend c "red"
  } else {
    lappend c "green"
  }
}

set model [load_charts_model -tcl [list $x $y $c] -first_line_header]

set_charts_data -model $model -column 2 -name column_type -value {{color}}

for {set i 0} {$i < 100} {incr i} {
  set ci [lindex $c $i]

  set_charts_data -model $model -column 1 -row $i -name value -value $ci -role 256
}

set plot [create_charts_plot -type scatter -model $model \
  -columns {{x X} {y Y} {color 1@256}} -title "Scatter"]
