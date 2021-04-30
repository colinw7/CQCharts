# Parallel Plot

set model1 [load_charts_model -csv data/diamonds.csv -first_line_header]

set plot1 [create_charts_plot -model $model1 -type parallel \
  -columns {{x cut} {y {carat color clarity depth table price x y z}}} -title "parallel"]
