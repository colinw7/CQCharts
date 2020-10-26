# load model
set model [load_charts_model -csv data/who_suicide_statistics.csv -first_line_header]

# display strip plot of suicides per gender
set view [create_charts_view]

set plot1 [create_charts_plot -model $model -type strip -columns {{value suicides_no} {name year} {position year}}]
set plot2 [create_charts_plot -model $model -type strip -columns {{value suicides_no} {name sex}}]

place_charts_plots -view $view -rows 2 [list $plot1 $plot2]

