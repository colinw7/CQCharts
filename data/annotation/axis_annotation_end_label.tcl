set plot [create_charts_plot -type empty]

set axis [create_charts_axis_annotation -plot $plot \
 -direction horizontal -start -0.8 -end 0.8 -position 0.0]

set_charts_property -annotation $axis -name label.text.string -value "Axis Label"
