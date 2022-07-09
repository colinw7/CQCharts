set model [load_charts_model -csv data/Sheet_2.csv -first_line_header \
  -column_type {{{1 integer} {format %d%%}} {{2 integer} {format %d%%}}}]
#set model [load_charts_model -csv data/pie_trend.csv -first_line_header]

set plot [create_charts_plot -model $model -type pie \
  -columns {{values {{1} {2}}} {group 0}} \
  -title "pie chart"]

set_charts_property -plot $plot -name options.maxValue -value 100

set_charts_property -plot $plot -name dataGrouping.rowGroups    -value 1
set_charts_property -plot $plot -name dataGrouping.bucket.exact -value 1
