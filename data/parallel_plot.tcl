# Parallel Plot

set model1 [load_charts_model -csv data/parallel_coords.csv -first_line_header]

set plot1 [create_charts_plot -model $model1 -type parallel \
  -columns {{x 0} {y {1 2 3 4 5 6 7}}} -title "parallel"]

#set model2 [load_charts_model -csv data/radar.csv -first_line_header]

#set plot2 [create_charts_plot -model $model2 -type parallel \
# -columns {{x 0} {y {1 2 3 4 5}}} -title "radar"]
