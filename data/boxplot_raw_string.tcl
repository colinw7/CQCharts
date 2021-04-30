set model [load_charts_model -csv data/boxplot_raw.csv -first_line_header]

set plot [create_charts_plot -model $model -type boxplot \
  -columns {{x 0} {min 1} {lowerMedian 2} {median 3} {upperMedian 4} {max 5}}]

