set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot1 [create_charts_plot -type scatter -columns {{x 0} {y 1}} -title "New York Temperatures"]
set plot2 [create_charts_plot -type scatter -columns {{x 0} {y 2}} -title "San Francisco Temperatures"]
set plot3 [create_charts_plot -type scatter -columns {{x 0} {y 3}} -title "Austin Temperatures"]

group_charts_plots -overlay [list $plot1 $plot2 $plot3]
