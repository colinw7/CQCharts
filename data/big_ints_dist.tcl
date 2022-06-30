set model1 [load_charts_model -csv data/big_ints.csv -first_line_header]
set model2 [load_charts_model -csv data/big_ints.csv -first_line_header \
 -column_type {{{1 real} {format %gM} {format_scale 0.000001}}}]

set plot1 [create_charts_plot -type distribution -model $model1 -columns {{values 1}}]
set plot2 [create_charts_plot -type distribution -model $model2 -columns {{values 1}}]

set view [get_charts_data -plot $plot1 -name view]

place_charts_plots -horizontal -view $view [list $plot1 $plot2]
