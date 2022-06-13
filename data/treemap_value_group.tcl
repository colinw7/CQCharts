set model [load_charts_model -csv data/value_group.csv -first_line_header]

set plot [create_charts_plot -model $model -type treemap \
  -columns {{value 0} {name 1}} -title "tree map"]
