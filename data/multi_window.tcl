set model1 [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{time {format %Y%m%d}}}]

set view1 [create_charts_view]
set view2 [create_charts_view]
set view3 [create_charts_view]

set plot1 [create_charts_plot -view $view1 -model $model1 -type xy -columns {{x 0} {y 1}}]
set plot2 [create_charts_plot -view $view2 -model $model1 -type xy -columns {{x 0} {y 2}}]
set plot3 [create_charts_plot -view $view3 -model $model1 -type xy -columns {{x 0} {y 3}}]
