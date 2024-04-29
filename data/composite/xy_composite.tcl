set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot1 [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "New York Temperatures"]
set plot2 [create_charts_plot -type xy -columns {{x 0} {y 2}} -title "San Francisco Temperatures"]
set plot3 [create_charts_plot -type xy -columns {{x 0} {y 3}} -title "Austin Temperatures"]

set_charts_property -plot $plot1 -name name -value "New York"
set_charts_property -plot $plot2 -name name -value "San Francisco"
set_charts_property -plot $plot3 -name name -value "Austin"

group_charts_plots -composite [list $plot1 $plot2 $plot3]
