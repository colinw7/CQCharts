set model [load_charts_model -csv data/multi_name_value.csv -first_line_header]

set plot1 [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "XY Plot 1"]
set plot2 [create_charts_plot -type xy -columns {{x 2} {y 3}} -title "XY Plot 1"]

set_charts_property -plot $plot1 -name columns.mapXColumn -value 1
set_charts_property -plot $plot2 -name columns.mapXColumn -value 1

set_charts_property -plot $plot1 -name xaxis.showOverlayAxes -value 1

group_charts_plots -overlay [list $plot1 $plot2]
