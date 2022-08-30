set model [load_charts_model -csv data/force_directed_node_color.csv -first_line_header]

set plot [create_charts_plot -model $model -type forcedirected \
  -columns {{from 0} {to 1} {value 2} {group 4} {attributes 3}}]
