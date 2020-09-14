set model [load_charts_model -csv CQTop.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart -columns {{name NAME} {value CPU}}]

set_charts_property -plot $plot -name filter.expression -value {$CPU > 0}
