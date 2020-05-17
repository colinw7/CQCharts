# Force Directed Plot

set model [load_charts_model -tsv data/adjacency.tsv -comment_header \
 -column_type {{{3 connection_list}}}]

set plot [create_charts_plot -model $model -type forcedirected -id forcedirected \
 -columns {{node 1} {connections 3} {name 0} {groupId 2}} -title "force directed"]
