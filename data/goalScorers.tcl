set model [load_model -csv data/goalScorers.csv -first_line_header -column_type "date#time:format=%Y-%m-%d,oformat=%F"]

set view [create_view]

set plot [create_plot -view $view -type xy \
 -columns "x=date,y=G,group=name" \
 -title "Goal Scorers" \
 -parameter "points=1" \
 -properties "points.count=2" \
 -properties "lines.selectable=1"]

set_charts_property -view $view -name showTable    -value 0
set_charts_property -view $view -name showSettings -value 0

set_charts_property -view $view -name select.highlight.stroke.color -value "palette"
set_charts_property -view $view -name select.highlight.stroke.width -value "3px"
set_charts_property -view $view -name inside.highlight.stroke.color -value "red"
set_charts_property -view $view -name inside.highlight.stroke.width -value "3px"

set_charts_property -plot $plot -name lines.color -value "interface:0.25"
set_charts_property -plot $plot -name lines.width -value "1px"
