# Force Directed Plot

set model1 [load_charts_model -tsv data/adjacency.tsv -comment_header \
 -column_type {{{3 connection_list}}}]

set plot1 [create_charts_plot -model $model1 -type forcedirected -id forcedirected \
 -columns {{node 1} {connections 3} {name 0} {groupId 2}} -title "force directed"]

#set model2 [load_charts_model -csv data/adjacency.csv -comment_header]

#set plot2 [create_charts_plot -model $model2 -type forcedirected \
# -columns {{namePair 0} {count 1} {groupId 2}} -title force directed]
