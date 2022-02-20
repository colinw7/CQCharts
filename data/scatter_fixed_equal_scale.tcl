set model [load_charts_model -csv data/scatter_100.csv -first_line_header]

set plot [create_charts_plot -type scatter -columns {{x 0} {y 1}} \
  -title "Scatter Fixed Size Equal Scaled"]

set_charts_property -plot $plot -name range.xmin -value 0
set_charts_property -plot $plot -name range.ymin -value 0
set_charts_property -plot $plot -name range.xmax -value 100
set_charts_property -plot $plot -name range.ymax -value 100
set_charts_property -plot $plot -name range.ymax -value 100

set_charts_property -plot $plot -name dataBox.unscaledRange -value 1

set_charts_property -plot $plot -name scaling.equal -value true
