set model [load_charts_model -csv data/adjacency_bad.csv -first_line_header]
#set model [load_charts_model -csv data/one_col.csv -first_line_header]

set plot [create_charts_plot -model $model -type adjacency \
  -columns {{node 0} {connections 1} {name 2} {link 3} {value 4} {group 5}}]
