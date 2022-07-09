set model [load_charts_model -csv data/periodic.csv -first_line_header]

set plot [create_charts_plot -model $model -type grid \
  -columns {{name Element} {row Period} {column Group} {color Type} {values AtomicNumber}} \
  -title "Periodic"]
