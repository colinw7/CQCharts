set x { X 0 1 }
set y { Y 0 1 }

set model [load_charts_model -tcl [list $x $y] -first_line_header]

set plot [create_charts_plot -type xy -columns {{x X} {y Y}} -title "X/Y"]

set_charts_property -plot $plot -name xaxis.visible -value 0
set_charts_property -plot $plot -name yaxis.visible -value 0

set axisId1 [create_charts_axis_annotation -plot $plot \
 -direction horizontal -start 0.0 -end 1.0 -position -0.05]
set axisId2 [create_charts_axis_annotation -plot $plot \
 -direction vertical -start 0.0 -end 1.0 -position -0.05]

set_charts_property -annotation $axisId1 -name label.text.string -value "Axis Horizontal"
set_charts_property -annotation $axisId2 -name label.text.string -value "Axis Vertical"

set_charts_property -annotation $axisId1 -name fitted -value 1
set_charts_property -annotation $axisId2 -name fitted -value 1
