# Y1Y2 Plots

set model1 [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{time {format %Y%m%d}}}]

set view1 [create_charts_view]
set view2 [create_charts_view]

set plot1 [create_charts_plot -view $view1 -model $model1 -type xy -columns {{x 0} {y 1}} \
  -title "multiple y axis"]
set plot2 [create_charts_plot -view $view2 -model $model1 -type xy -columns {{x 0} {y 2}}]

group_charts_plots -overlay -y1y2 [list $plot1 $plot2]
