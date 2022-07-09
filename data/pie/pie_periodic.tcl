set model [load_charts_model -csv data/periodic.csv -first_line_header]

set plot [create_charts_plot -model $model -type piechart \
  -columns {{values AtomicMass} {label Element} {group Type}} -title "Periodic"]
