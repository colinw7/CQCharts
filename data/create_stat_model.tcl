proc create_stat_model { model } {
  set nr [get_charts_data -model $model -name num_rows]
  set nc [get_charts_data -model $model -name num_columns]

  set stat_column(-1) [list {} mean min lower_median median upper_median max outliers]

  set c1 0

  for {set c 0} {$c < $nc} {incr c} {
    set type [get_charts_data -model $model -column $c -name details.type]

    if {$type != "real" && $type != "integer"} {
      continue
    }

    set name [get_charts_data -model $model -column $c -header -name value]

    set stat_column($c1) {}

    lappend stat_column($c1) $name
    lappend stat_column($c1) [get_charts_data -model $model -column $c -name details.mean]
    lappend stat_column($c1) [get_charts_data -model $model -column $c -name details.min]
    lappend stat_column($c1) [get_charts_data -model $model -column $c -name details.lower_median]
    lappend stat_column($c1) [get_charts_data -model $model -column $c -name details.median]
    lappend stat_column($c1) [get_charts_data -model $model -column $c -name details.upper_median]
    lappend stat_column($c1) [get_charts_data -model $model -column $c -name details.max]
    lappend stat_column($c1) [get_charts_data -model $model -column $c -name details.outliers]

    incr c1
  }

  set nc1 $c1

  set ::stat_columns {}

  lappend ::stat_columns $stat_column(-1)

  for {set c1 0} {$c1 < $nc1} {incr c1} {
    lappend ::stat_columns $stat_column($c1)
  }

  set stat_model [load_charts_model -var stat_columns -first_line_header -first_column_header]

  for {set c1 0} {$c1 < $nc1} {incr c1} {
    set_charts_data -model $stat_model -name column_type -column $c1 -value {{real}}
  }

  return $stat_model
}
