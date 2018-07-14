set model [load_model -csv data/winequality-white.csv -first_line_header -separator {;}]

#---

source describe_model.tcl

describe_model $model

#---

source create_stat_model.tcl

set stat_model [create_stat_model $model]

#---

set unique [get_charts_data -model $model -column 11 -name unique_values]
set counts [get_charts_data -model $model -column 11 -name unique_counts]

puts $unique
puts $counts

#---

set nr [get_charts_data -model $model -name num_rows]
set nc [get_charts_data -model $model -name num_columns]

set view1 [create_view]

set box_plots {}

for {set c 0} {$c < $nc} {incr c} {
  set plot [create_plot -view $view1 -model $model -type boxplot -columns "group=(0),value=$c"]

  lappend box_plots $plot

  set_property -plot $plot -name box.width                     -value 0.4
  set_property -plot $plot -name xaxis.visible                 -value 0
  set_property -plot $plot -name yaxis.grid.line.major.visible -value 1
  set_property -plot $plot -name key.visible                   -value 0
  set_property -plot $plot -name labels.visible                -value 0
  set_property -plot $plot -name box.fill.color                -value green
  set_property -plot $plot -name outlier.symbol.fill.color     -value green
}

place_plots -horizontal $box_plots

#---

set view2 [create_view]

set dist_plots {}

for {set c 0} {$c < $nc} {incr c} {
  set plot [create_plot -view $view2 -model $model -type distribution -columns "value=$c"]

  lappend dist_plots $plot

  set_property -plot $plot -name bucket.auto                   -value 1
  set_property -plot $plot -name key.visible                   -value 0
  set_property -plot $plot -name yaxis.grid.line.major.visible -value 1
  set_property -plot $plot -name yaxis.label.visible           -value 0
}

place_plots -horizontal $dist_plots

#---

set corrModel [correlation_model -model $model]

set view3 [create_view]

set plot3 [create_plot -view $view3 -model $corrModel -type image]

set_property -plot $plot3 -name labels.x    -value 1
set_property -plot $plot3 -name labels.y    -value 1
set_property -plot $plot3 -name labels.cell -value 1
set_property -plot $plot3 -name invert.y    -value 1

#---

if {0} {
for {set c 0} {$c < $nc} {incr c} {
  puts "  CMathCorrelation::Values c$c = {"
 
  for {set r 0} {$r < $nr} {incr r} {
    set v [get_charts_data -model $model -column $c -row $r -name value]

    if {$r > 0} {
      puts -nonewline ", "
    }

    puts -nonewline $v
  }

  puts ""
  puts "};"
}
}
