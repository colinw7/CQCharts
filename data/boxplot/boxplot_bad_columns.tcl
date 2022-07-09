set model [load_charts_model -csv data/boxplot_raw.csv -first_line_header]

set plot [create_charts_plot -model $model -type boxplot \
  -columns {{x 0} {min 0} {lowerMedian 1} {median 2} {upperMedian 3} {max 4}}]

set plot [create_charts_plot -model $model -type boxplot \
  -columns {{x 0} {min 1} {lowerMedian 0} {median 2} {upperMedian 3} {max 4}}]

