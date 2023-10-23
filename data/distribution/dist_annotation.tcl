set model [load_charts_model -csv data/gaussian.txt -comment_header]

set plot [create_charts_plot -model $model -type distribution \
  -columns {{values 0}} -title "distribution"]

set line [create_charts_polyline_annotation -plot $plot \
 -points "10 0 10 300 sP"]
set line [create_charts_polyline_annotation -plot $plot \
 -points "20 0 20 300 sP"]
set line [create_charts_polyline_annotation -plot $plot \
 -points "30 0 30 300 sP"]

set_charts_property -plot $plot -name options.extraValues -value "10 20 30"
