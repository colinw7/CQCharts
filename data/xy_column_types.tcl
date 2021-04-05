# scatter plot with columns of all different types

set model [load_charts_model -csv data/column_types.csv -first_line_header]

set plot [create_charts_plot -model $model -type xy \
  -columns {{x x} {y y} {label label} {color color} {symbolType symbol_type} {symbolSize symbol_size} {fontSize font_size}}]
