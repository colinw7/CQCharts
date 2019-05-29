set model [load_charts_model -csv data/long_labels.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart -columns {{name 0} {value 1}}]
