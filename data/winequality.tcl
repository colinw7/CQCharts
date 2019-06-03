set model [load_charts_model -csv data/winequality-white.csv -first_line_header -separator {;}]

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

set view1 [create_charts_view]

set box_plots {}

for {set c 0} {$c < $nc} {incr c} {
  set plot [create_charts_plot -view $view1 -model $model -type boxplot \
    -columns [list [list group "(0)"] [list value $c]]]

  lappend box_plots $plot

  set_charts_property -plot $plot -name box.width                 -value 0.4
  set_charts_property -plot $plot -name xaxis.visible             -value 0
  set_charts_property -plot $plot -name yaxis.grid.lines          -value MAJOR
  set_charts_property -plot $plot -name key.visible               -value 0
  set_charts_property -plot $plot -name labels.visible            -value 0
  set_charts_property -plot $plot -name box.fill.color            -value green
  set_charts_property -plot $plot -name outlier.symbol.fill.color -value green
}

place_charts_plots -horizontal $box_plots

#---

set view2 [create_charts_view]

set dist_plots {}

for {set c 0} {$c < $nc} {incr c} {
  set plot [create_charts_plot -view $view2 -model $model -type distribution \
    -columns [list [list value $c]]]

  lappend dist_plots $plot

  set_charts_property -plot $plot -name bucket.auto              -value 1
  set_charts_property -plot $plot -name key.visible              -value 0
  set_charts_property -plot $plot -name yaxis.grid.lines         -value MAJOR
  set_charts_property -plot $plot -name yaxis.label.text.visible -value 0
}

place_charts_plots -horizontal $dist_plots

#---

set corrModel [create_charts_correlation_model -model $model]

set view3 [create_charts_view]

set plot3 [create_charts_plot -view $view3 -model $corrModel -type image]

set_charts_property -plot $plot3 -name xaxis.text.visible -value 1
set_charts_property -plot $plot3 -name yaxis.text.visible -value 1
set_charts_property -plot $plot3 -name cell.text.visible  -value 1
set_charts_property -plot $plot3 -name invert.y           -value 1

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
