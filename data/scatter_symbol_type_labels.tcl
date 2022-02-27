set model [load_charts_model -csv data/scatter_labels.csv -first_line_header]

#---

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {symbolType 2}} \
  -title "Scatter Symbol Type"]

set_charts_property -plot $plot -name mapping.symbolType.enabled -value 1

set_charts_property -plot $plot -name points.size -value 12px
