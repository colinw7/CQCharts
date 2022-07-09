set symbol_data [list \
 [list ind value color state] \
 [list 1 10 red bad] \
 [list 2 20 green good] \
 [list 3 30 green good] \
 [list 4 20 orange normal] \
 [list 5 15 orange normal] \
 [list 6 25 blue outlier] \
 [list 7 35 blue outlier] \
 [list 8 20 red bad] \
 [list 9 5  red bad] \
]

set model [load_charts_model -var symbol_data -transpose -first_line_header]

set_charts_data -model $model -column 2 -name column_type -value color

set plot [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {color 2} {colorLabel 3} {label 3}}]

set_charts_property -plot $plot -name mapping.color.enabled -value 0
