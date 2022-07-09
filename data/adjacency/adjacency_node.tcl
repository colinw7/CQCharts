set model [load_charts_model -tsv data/adjacency.tsv -comment_header \
 -column_type {{{3 connection_list}}}]

set plot [create_charts_plot -model $model -type adjacency \
  -columns {{node 1} {connections 3} {name 0} {group 2}} -title "adjacency"]

set objs [get_charts_data -plot $plot -name objects -sync]

#assert {[llength $objs] == 585}
