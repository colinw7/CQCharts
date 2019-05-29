set model [load_charts_model -csv data/gaussian.txt -comment_header]

set plot [create_charts_plot -model $model -type distribution \
  -columns {{value 0}} -title "distribution"]
