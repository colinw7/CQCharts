set model [load_charts_model -csv data/ages.csv -first_line_header]

set plot [create_charts_plot -type scatter -columns {{x 0} {y 1} {label 0}} \
  -title "Scatter Plot Label)"]
