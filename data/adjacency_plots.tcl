# Adjacency Plot

set model1 [load_charts_model -tsv data/adjacency.tsv -comment_header \
 -column_type {{{3 connection_list}}}]

set plot1 [create_charts_plot -model $model1 -type adjacency \
  -columns {{node 1} {connections 3} {name 0} {groupId 2}} -title "adjacency"]

#set model2 [load_charts_model -csv data/adjacency.csv -comment_header]

#set plot2 [create_charts_plot -model $model2 -type adjacency \
#  -columns {{namePair 0} {count 1} {groupId 2}} -title "adjacency"]

#place_charts_plots -horizontal $plot1 $plot2
