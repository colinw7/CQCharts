set model [load_charts_model -csv data/scatter_extra.csv -comment_header]

set_charts_data -model $model -column 0 -name column_type \
      -value {{real} {extra_values {0.0 1.0}}}
set_charts_data -model $model -column 1 -name column_type \
      -value {{real} {extra_values {0.0 1.0}}}

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1}} -title "extra scatter"]
