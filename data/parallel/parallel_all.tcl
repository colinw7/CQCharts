# Parallel Plot

set model1 [load_charts_model -csv data/bubble.csv -comment_header]

set plot1 [create_charts_plot -model $model1 -type parallel \
  -columns {{y {0 1 2 3 4}}} -title "parallel"]
