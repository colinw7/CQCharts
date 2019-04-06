set model [load_charts_model -csv data/boxplot.csv -first_line_header]

set view [create_charts_view]

set plot [create_charts_plot -view $view -type xy]

remove_charts_plot -plot $plot

remove_charts_view -view $view
