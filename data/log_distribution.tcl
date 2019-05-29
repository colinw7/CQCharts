set model [load_charts_model -csv data/diamonds.csv -first_line_header]

set view [create_charts_view]

set plot1 [create_charts_plot -view $view -model $model -type distribution -columns {{value cut}}]

set_charts_property -plot $plot1 -name "yaxis.valueType" -value "LOG"
