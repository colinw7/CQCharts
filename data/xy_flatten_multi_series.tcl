set model [load_charts_model -csv data/flatten_multi_series.csv -first_line_header \
  -column_type {{{time} {format %Y-%m-%d} {oformat %F}}}]

set plot [create_charts_plot -type xy -columns {{x 0} {y 2} {group 1}} -title "Temperatures"]

set_charts_property -plot $plot -name dataGrouping.split -value 1
set_charts_property -plot $plot -name points.visible -value 1
