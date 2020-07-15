set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot1 [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "New York Temperatures"]
set plot2 [create_charts_plot -type xy -columns {{x 0} {y 2}} -title "San Francisco Temperatures"]
set plot3 [create_charts_plot -type xy -columns {{x 0} {y 3}} -title "Austin Temperatures"]

set_charts_property -plot $plot1 -name points.visible -value 1
set_charts_property -plot $plot2 -name points.visible -value 1
set_charts_property -plot $plot3 -name points.visible -value 1

set_charts_property -plot $plot1 -name key.drawLine -value 1
set_charts_property -plot $plot2 -name key.drawLine -value 1
set_charts_property -plot $plot3 -name key.drawLine -value 1

group_charts_plots -overlay [list $plot1 $plot2 $plot3]
