# Force Directed Plot

set model1 [load_charts_model -tsv data/includes.tsv -comment_header \
 -column_type {{{2 connection_list}}}]

set plot1 [create_charts_plot -model $model1 -type forcedirected \
  -columns {{node 1} {connections 2} {name 0}} -title "includes"]
#set plot1 [create_charts_plot -model $model1 -type adjacency \
# -columns {{node 1} {connections 2} {name 0}} -title "includes"]
