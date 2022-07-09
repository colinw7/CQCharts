# Adjacency Plot

set model [load_charts_model -csv data/adjacency.csv -comment_header]

set plot [create_charts_plot -model $model -type adjacency \
  -columns {{link 0} {value 1} {group 2}} -title "adjacency"]

set objs [get_charts_data -plot $plot -name objects -sync]

#assert {[llength $objs] == 585}
