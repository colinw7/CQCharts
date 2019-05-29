set model [load_charts_model -csv data/goalScorers.csv -first_line_header -column_type "date#time:format=%Y-%m-%d,oformat=%F"]

set view [create_charts_view]

set plot [create_charts_plot -view $view -type xy \
 -columns {{x date} {y G} {group name}} \
 -columns {{tips {age mins played G NPG G90 NPG90}}} \
 -title "Goal Scorers" \
 -parameter "points=1"]

set_charts_property -view $view -name showTable    -value 0
set_charts_property -view $view -name showSettings -value 0

set_charts_property -view $view -name select.highlight.stroke.color -value "palette"
set_charts_property -view $view -name select.highlight.stroke.width -value "3px"
set_charts_property -view $view -name inside.highlight.stroke.color -value "red"
set_charts_property -view $view -name inside.highlight.stroke.width -value "3px"

set_charts_property -plot $plot -name points.count -value 1
set_charts_property -plot $plot -name points.start -value -1

set_charts_property -plot $plot -name lines.selectable -value 1

set_charts_property -plot $plot -name margin.inner.left   -value 16px
set_charts_property -plot $plot -name margin.inner.right  -value 16px
set_charts_property -plot $plot -name margin.inner.bottom -value 16px
set_charts_property -plot $plot -name margin.inner.top    -value 16px

set_charts_property -plot $plot -name lines.color -value "interface:0.25"
set_charts_property -plot $plot -name lines.width -value "1px"

set_charts_property -plot $plot -name key.header.text   -value "Goal Scorer"
set_charts_property -plot $plot -name key.scrollHeight  -value "256px"
set_charts_property -plot $plot -name key.pressBehavior -value "SELECT"
set_charts_property -plot $plot -name key.fill.visible  -value 1
