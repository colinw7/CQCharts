set model [load_charts_model -csv data/ages.csv -first_line_header]

set plot [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "XY Plot (X String)"]

#set_charts_property -plot $plot -name columns.mapXColumn -value 1
