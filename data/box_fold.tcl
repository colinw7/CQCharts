set model [load_charts_model -csv data/boxplot.csv -first_line_header]

set fold_model [fold_charts_model -model $model -column 0]

set tvars [process_charts_model -model $fold_model -analyze -type barchart]

set columns {}

foreach tvar $tvars {
  set nameColumns [lindex $tvar 1]

  if {[llength $nameColumns] > 0} {
    foreach nameColumn $nameColumns {
      set name   [lindex $nameColumn 0]
      set column [lindex $nameColumn 1]

      lappend columns [list $name $column]
    }
  }
}

echo $columns

set plot [create_charts_plot -type barchart -model $fold_model -columns $columns]
