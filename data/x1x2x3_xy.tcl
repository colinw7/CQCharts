# X1X2 Plots

set model1 [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{time {format %Y%m%d}}}]

set plot1 [create_charts_plot -model $model1 -type xy -columns {{x 1} {y 0}} \
  -title "multiple x axis"]
set plot2 [create_charts_plot -model $model1 -type xy -columns {{x 2} {y 0}}]
set plot3 [create_charts_plot -model $model1 -type xy -columns {{x 3} {y 0}}]

group_charts_plots -overlay -x1x2 [list $plot1 $plot2 $plot3]
