set model1 [load_charts_model -tsv data/multi_series_1.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]
set model2 [load_charts_model -tsv data/multi_series_2.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]
set model3 [load_charts_model -tsv data/multi_series_3.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot1 [create_charts_plot -model $model1 -type xy \
  -columns {{x 0} {y 1}} -title "New York Temperatures"]
set plot2 [create_charts_plot -model $model2 -type xy \
  -columns {{x 0} {y 1}} -title "San Francisco Temperatures"]
set plot3 [create_charts_plot -model $model3 -type xy \
  -columns {{x 0} {y 1}} -title "Austin Temperatures"]

group_charts_plots -overlay [list $plot1 $plot2 $plot3]
