set model [load_charts_model -csv data/periodic.csv -first_line_header]

set plot [create_charts_plot -model $model -type dendrogram \
  -columns {{value AtomicMass} {name Element} {group Type}} -title "Periodic"]
