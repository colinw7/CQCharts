# Force Direct From/To
set model [load_charts_model -csv data/dot/dot_philo.csv -first_line_header]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{from 0} {to 1}} -title "force directed philo"]
