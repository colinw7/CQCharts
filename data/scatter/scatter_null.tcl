set model [load_charts_model -csv data/null_values.csv -first_line_header]

set_charts_data -model $model -column 0 -name column_type -value {integer}
set_charts_data -model $model -column 1 -name column_type -value {real}
set_charts_data -model $model -column 2 -name column_type -value {integer}
set_charts_data -model $model -column 3 -name column_type -value {string}

set plot [create_charts_plot -type scatter -columns {{x 0} {y 1}}]
