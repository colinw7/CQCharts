# Force Directed Path
set model [load_charts_model -csv data/flare.csv -comment_header]

set plot [create_charts_plot -model $model -type force_directed \
  -columns {{path 0} {value 1}} -title "chord path"]
