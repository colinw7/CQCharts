set model [load_charts_model -tsv data/coffee.tsv -first_line_header]

set plot [create_charts_plot -model $model -type treemap \
  -columns {{name 0} {color 1}} -title "coffee characteristics"]

set_charts_property -plot $plot -name header.visible -value 0
