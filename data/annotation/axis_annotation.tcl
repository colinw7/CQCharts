set x { X 0 1 }
set y { Y 0 1 }

set model [load_charts_model -tcl [list $x $y] -first_line_header]

set plot [create_charts_plot -type xy -columns {{x X} {y Y}} -title "X/Y"]

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0

set axisId1 [create_charts_axis_annotation -plot $plot \
 -direction horizontal -start 0.2 -end 0.8 -position 0.5]
set axisId2 [create_charts_axis_annotation -plot $plot \
 -direction vertical -start 0.2 -end 0.8 -position 0.5]

set_charts_property -annotation $axisId1 -name label.text.string -value "Axis Horizontal"
set_charts_property -annotation $axisId2 -name label.text.string -value "Axis Vertical"
