set model [load_charts_model -tsv data/hathi_data.tsv -first_line_header]

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1}}]
