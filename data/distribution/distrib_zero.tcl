set model [load_charts_model -csv data/distrib_zero.csv -comment_header]

set plot [create_charts_plot -model $model -type distribution \
  -columns {{values 0}}]
