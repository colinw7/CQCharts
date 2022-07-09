set model [load_charts_model -csv data/value_group.csv -first_line_header]

set plot [create_charts_plot -model $model -type piechart \
  -columns {{values 0} {label 1} {group 2}} -title "piechart"]
