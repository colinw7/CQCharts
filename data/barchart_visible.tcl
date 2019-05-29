set model [load_charts_model -csv data/multi_bar.csv -first_line_header]

set vis [process_charts_model -model $model -add -expr "1" -header "Visible" -type boolean]

set plot [create_charts_plot -model $model -type barchart \
  -columns [list [list name 0] [list value 1] [list visible $vis]]]
