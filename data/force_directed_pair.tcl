# Force Directed Plot

set model [load_charts_model -csv data/adjacency.csv -comment_header]

set plot [create_charts_plot -model $model -type forcedirected \
 -columns {{namePair 0} {count 1} {groupId 2}} -title force directed]
