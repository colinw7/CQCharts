set model [load_model -csv data/winequality-white.csv -first_line_header -separator {;}]

set nr [get_charts_data -model $model -name num_rows]
set nc [get_charts_data -model $model -name num_columns]

puts "($nr, $nc)"

for {set c 0} {$c < $nc} {incr c} {
  set name [get_charts_data -model $model -column $c -header -name value]

  set num_null [get_charts_data -model $model -column $c -name num_null]

  set num_non_null [expr {$nr - $num_null}]

  set type [get_charts_data -model $model -column $c -name type]

  puts "$name\t$num_non_null non-null $type"
}

puts ""

set stat_column(-1) [list {} mean min lower_median median upper_median max]

for {set c 0} {$c < $nc} {incr c} {
  set name [get_charts_data -model $model -column $c -header -name value]

  set stat_column($c) {}

  lappend stat_column($c) $name
  lappend stat_column($c) [get_charts_data -model $model -column $c -name mean]
  lappend stat_column($c) [get_charts_data -model $model -column $c -name min]
  lappend stat_column($c) [get_charts_data -model $model -column $c -name lower_median]
  lappend stat_column($c) [get_charts_data -model $model -column $c -name median]
  lappend stat_column($c) [get_charts_data -model $model -column $c -name upper_median]
  lappend stat_column($c) [get_charts_data -model $model -column $c -name max]

  puts "$stat_column($c)"
}

set stat_columns {}

lappend stat_columns $stat_column(-1)

for {set c 0} {$c < $nc} {incr c} {
  lappend stat_columns $stat_column($c)
}

set stat_model [load_model -var stat_columns -first_line_header -first_column_header]

puts ""

set unique [get_charts_data -model $model -column 11 -name unique_values]
set counts [get_charts_data -model $model -column 11 -name unique_counts]

puts $unique
puts $counts

#---

set view1 [create_view]

set box_plots {}

for {set c 0} {$c < $nc} {incr c} {
  set plot [create_plot -view $view1 -model $model -type boxplot -columns "x=(0),y=$c" -xintegral 1]

  lappend box_plots $plot

  set_property -plot $plot -name box.width                     -value 0.4
  set_property -plot $plot -name xaxis.visible                 -value 0
  set_property -plot $plot -name yaxis.grid.line.major.visible -value 1
  set_property -plot $plot -name key.visible                   -value 0
  set_property -plot $plot -name labels.visible                -value 0
  set_property -plot $plot -name box.fill.color                -value green
}

place_plots -horizontal $box_plots

#---

set view2 [create_view]

set dist_plots {}

for {set c 0} {$c < $nc} {incr c} {
  set plot [create_plot -view $view2 -model $model -type distribution -columns "value=$c"]

  lappend dist_plots $plot

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
