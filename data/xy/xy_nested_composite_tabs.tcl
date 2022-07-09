set model [load_charts_model -tsv data/multi_series.tsv -comment_header \
  -column_type {{{time} {format %Y%m%d} {oformat %F}}}]

set plot1 [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "New York Temperatures"]
set plot2 [create_charts_plot -type xy -columns {{x 0} {y 2}} -title "San Francisco Temperatures"]
set plot3 [create_charts_plot -type xy -columns {{x 0} {y 3}} -title "Austin Temperatures"]

set_charts_property -plot $plot1 -name name -value "New York"
set_charts_property -plot $plot2 -name name -value "San Francisco"
set_charts_property -plot $plot3 -name name -value "Austin"

set composite1 [group_charts_plots -composite -tabbed [list $plot1 $plot2 $plot3]]

set plot4 [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "New York Temperatures"]
set plot5 [create_charts_plot -type xy -columns {{x 0} {y 2}} -title "San Francisco Temperatures"]
set plot6 [create_charts_plot -type xy -columns {{x 0} {y 3}} -title "Austin Temperatures"]

set_charts_property -plot $plot4 -name name -value "New York"
set_charts_property -plot $plot5 -name name -value "San Francisco"
set_charts_property -plot $plot6 -name name -value "Austin"

set composite2 [group_charts_plots -composite -tabbed [list $plot4 $plot5 $plot6]]

group_charts_plots -composite -tabbed [list $composite1 $composite2]
