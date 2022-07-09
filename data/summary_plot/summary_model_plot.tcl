set model [load_charts_model -csv data/diamonds.csv -first_line_header]

set model1 [create_charts_summary_model -model $model -sorted]

set view [create_charts_view]

set plot [create_charts_plot -view $view -model $model1 -type scatter \
 -columns {{x carat} {y price}}]
