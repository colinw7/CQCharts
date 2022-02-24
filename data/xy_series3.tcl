set model [load_charts_model -csv data/series3.csv -first_line_header]

set plot [create_charts_plot -type xy -columns {{x 0} {y {1 2 3}}} -title "XY Plot"]

set_charts_property -plot $plot -name columns.label -value {4}
set_charts_property -plot $plot -name points.adjustText -value {true}
set_charts_property -plot $plot -name points.labels.visible -value {true}
