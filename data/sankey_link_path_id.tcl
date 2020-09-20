# sankey link model
set model [load_charts_model -csv data/sankey_link_path_id.csv -first_line_header]

set plot [create_charts_plot -model $model -type sankey \
 -columns {{link Link} {value Value} {depth Depth} {attributes Attributes}}]

set_charts_property -plot $plot -name plotBox.clip -value 0
set_charts_property -plot $plot -name dataBox.clip -value 0

set axis [create_charts_axis_annotation -plot $plot \
  -start -1 -end 1 -position -1 -direction horizontal]

set_charts_property -annotation $axis -name valueStart     -value 1
set_charts_property -annotation $axis -name valueEnd       -value 5
set_charts_property -annotation $axis -name majorIncrement -value 1
