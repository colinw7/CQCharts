proc rand_in { min max } {
  set d [expr {$max - $min}]

  return [expr {rand()*$d + $min}]
}

set n 100

set model [create_charts_data_model -rows $n -columns 3]

set_charts_data -model $model -column 0 -header -name value -value A
set_charts_data -model $model -column 1 -header -name value -value B
set_charts_data -model $model -column 2 -header -name value -value C

for {set r 0} {$r < $n} {incr r} {
  set x [rand_in 0 100]
  set y [rand_in 0 100]
  set c [expr {int([rand_in 0 10])}]

  if {$c == 5} {
    set y NaN
  }

  set_charts_data -model $model -column 0 -row $r -name value -value $x
  set_charts_data -model $model -column 1 -row $r -name value -value $y
  set_charts_data -model $model -column 2 -row $r -name value -value $c
}

set plot [create_charts_plot -model $model -type scatter -columns {{x A} {y B} {color C}}]
