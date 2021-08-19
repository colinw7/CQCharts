set model [load_charts_model -csv data/pie_value_set.csv -first_line_header]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model -type pie \
  -columns {{label 0} {values 1}} -title "pie chart"]
