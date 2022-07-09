set model [load_charts_model -csv data/long_labels.csv -first_line_header]

set plot [create_charts_plot -model $model -type bubble -columns {{name 0} {value 1}}]

set_charts_property -plot $plot -name text.clipLength -value "100px"
