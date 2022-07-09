set model [load_charts_model -csv data/value_group.csv -first_line_header]

set plot [create_charts_plot -model $model -type sunburst \
  -columns {{value 0} {name 1} {group 2}} -title "sunburst"]
