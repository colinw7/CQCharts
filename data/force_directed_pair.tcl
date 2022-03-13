# link column model
set model [load_charts_model -csv data/adjacency.csv -comment_header]

set plot [create_charts_plot -model $model -type forcedirected \
 -columns {{link 0} {value 1} {group 2}} -title force directed]

#set_charts_property -plot $plot -name options.initSteps -value 500
#set_charts_property -plot $plot -name options.running   -value 0
