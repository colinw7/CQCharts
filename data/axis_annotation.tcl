set x { X 0 1 }
set y { Y 0 1 }

set modelId [load_charts_model -tcl [list $x $y] -first_line_header]

set plotId [create_charts_plot -type xy -columns {{x X} {y Y}} -title "X/Y"]

set axisId1 [create_charts_axis_annotation -plot $plotId \
 -direction vertical -start 0.0 -end 1.0 -position 1.0]

set_charts_property -annotation $axisId1 -name label.text.string -value "Axis Annotation"
