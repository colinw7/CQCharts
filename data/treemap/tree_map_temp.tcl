set model [load_charts_model -csv data/temp_hier.csv -first_line_header -column_type {{{1 real}}}]

set plot [create_charts_plot -model $model -type treemap \
  -columns {{name 0} {value 1} {group 2}} -title "hierarchical tree"]
