set model [load_charts_model -csv data/dot/dot_philo.csv -first_line_header]

set plot [create_charts_plot -model $model -type graphviz \
  -columns {{from 0} {to 1} {group 3}} -title "graphviz philo"]
