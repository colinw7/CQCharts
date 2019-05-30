set model [load_charts_model -csv data/economics.csv -first_line_header \
  -column_type {{{0 time} {format %Y-%m-%d}}}]

#set plot [create_charts_plot -model $model -type xy -columns {{x date} {y pop}}]

set plot [create_charts_plot -model $model -type xy -columns {{x date} {y {psavert uempmed}}}]
