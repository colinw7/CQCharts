proc modelChanged { model } {
  echo "modelChange $model"

  write_charts_model -model $model
}

set model [load_charts_model -csv data/ToothGrowth.csv -first_line_header]

connect_charts_signal -model $model -from modelChanged -to modelChanged

set_charts_data -model $model -name read_only -value 0

set_charts_data -model $model -row 0 -column 0 -name value -value Test
