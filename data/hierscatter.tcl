set model [load_charts_model -csv data/airports.csv -comment_header]

set plot [create_charts_plot -model $model -type hierscatter \
  -columns {{x 6} {y 5} {name 1} {group {4 3 2}}} -title "airports"]
