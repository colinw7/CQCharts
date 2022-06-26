set model [create_charts_data_model -rows 5 -columns 2]

set nr [get_charts_data -model $model -name num_rows]
set nc [get_charts_data -model $model -name num_columns]

for {set r 0} {$r < $nr} {incr r} {
  set_charts_data -model $model -name value -row $r -column 0 -value 1
  set_charts_data -model $model -name value -row $r -column 1 -value $r
}

set plot [create_charts_plot -model $model -type bubble -columns {{value 0} {name 1}}]
