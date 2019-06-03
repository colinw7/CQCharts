set model [load_charts_model -csv data/temp_bar_range.csv -first_line_header]

set plot [create_charts_plot -type barchart -model $model -columns {{group 0} {value {2 3}}} \
  -parameter {valueType RANGE} -parameter {horizontal 1} -properties {{dataLabel.visible 1}} \
  -properties {{dataLabel.position TOP_OUTSIDE}} -properties {{key.visible 0}} \
  -properties {{margins.bar 12px}} -properties {{fill.color {palette 0.2}}}]
