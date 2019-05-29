set model [load_charts_model -csv data/winequality-white.csv -first_line_header -separator {;}]

#---

proc create_stat_model { model } {
  set nr [get_charts_data -model $model -name num_rows]
  set nc [get_charts_data -model $model -name num_columns]

  set ::stat_rows { }

  lappend stat_row ""
  lappend stat_row mean
  lappend stat_row min
  lappend stat_row lower_median
  lappend stat_row median
  lappend stat_row upper_median
  lappend stat_row max
  lappend stat_row outliers
  lappend stat_row stddev

  lappend ::stat_rows $stat_row

  for {set c 0} {$c < $nc} {incr c} {
    set type [get_charts_data -model $model -column $c -name type]

    if {$type != "real" && $type != "integer"} {
      continue
    }

    set name [get_charts_data -model $model -column $c -header -name value]

    set stat_row {}

    lappend stat_row $name
    lappend stat_row [get_charts_data -model $model -column $c -name mean]
    lappend stat_row [get_charts_data -model $model -column $c -name min]
    lappend stat_row [get_charts_data -model $model -column $c -name lower_median]
    lappend stat_row [get_charts_data -model $model -column $c -name median]
    lappend stat_row [get_charts_data -model $model -column $c -name upper_median]
    lappend stat_row [get_charts_data -model $model -column $c -name max]
    lappend stat_row [get_charts_data -model $model -column $c -name outliers]
    lappend stat_row [get_charts_data -model $model -column $c -name stddev]

    lappend ::stat_rows $stat_row
  }

  set stat_model [load_charts_model -var ::stat_rows -first_line_header -first_column_header -transpose]

  return $stat_model
}

set stat_model [create_stat_model $model]

set plot [create_charts_plot -model $stat_model -type boxplot \
  -columns {{min 1} {lowerMedian 2} {median 3} {upperMedian 4} {max 5} {outliers 6}} \
  -properties {{options.horizontal 1} {options.normalized 1}}]
