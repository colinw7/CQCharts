set model [load_charts_model -csv data/big_int.csv -first_line_header \
  -column_type {{{0 integer}} {{1 integer}}}]

#set plot [create_charts_plot -type scatter -columns {{x 0} {y 1}} -title "Big Int"]
set plot [create_charts_plot -type barchart -columns {{name 0} {values 1}} -title "Big Int"]
