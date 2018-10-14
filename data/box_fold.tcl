set model [load_model -csv data/boxplot.csv -first_line_header]

set fold_model [fold_model -model $model -column 0]

set tvars [process_model -model $fold_model -analyze -type barchart]

set columns ""

foreach tvar $tvars {
  set nameColumns [lindex $tvar 1]

  if {[llength $nameColumns] > 0} {
    set i 0

    foreach nameColumn $nameColumns {
      set name   [lindex $nameColumn 0]
      set column [lindex $nameColumn 1]

      if {$i > 0} {
        append columns ","
      }

      append columns "$name=$column"

      incr i
    }
  }
}

echo $columns

set plot [create_plot -type barchart -model $fold_model -columns $columns]
