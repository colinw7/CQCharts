set model [load_charts_model -csv data/value_group.csv -first_line_header]

set plot [create_charts_plot -model $model -type barchart \
  -columns {{values 0}} -title "barchart"]
