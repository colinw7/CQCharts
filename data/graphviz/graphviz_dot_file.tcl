set model [load_charts_model -csv data/empty.csv]

set plot [create_charts_plot -model $model -type graphviz]

#set_charts_property -plot $plot -name dot.file -value dot/fsm.dot
set_charts_property -plot $plot -name dot.file -value dot/philo.dot
