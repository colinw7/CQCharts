set model [load_charts_model -csv data/log.csv -comment_header]

set plot1 [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Log Scale"]
set plot2 [create_charts_plot -type xy -columns {{x 0} {y 1}} -title "Linear Scale"]

set_charts_property -plot $plot1 -name yaxis.valueType            -value LOG
set_charts_property -plot $plot1 -name lines.stroke.width         -value 2px
set_charts_property -plot $plot1 -name lines.stroke.color         -value {palette 0.25}
set_charts_property -plot $plot1 -name points.visible             -value 1
set_charts_property -plot $plot1 -name points.symbol.type         -value circle
set_charts_property -plot $plot1 -name points.symbol.fill.visible -value 1
set_charts_property -plot $plot1 -name points.symbol.fill.color   -value {palette 0.25}

set_charts_property -plot $plot2 -name lines.stroke.width         -value 2px
set_charts_property -plot $plot2 -name lines.stroke.color         -value {palette 0.75}
set_charts_property -plot $plot2 -name points.visible             -value 1
set_charts_property -plot $plot2 -name points.symbol.type         -value circle
set_charts_property -plot $plot2 -name points.symbol.fill.visible -value 1
set_charts_property -plot $plot2 -name points.symbol.fill.color   -value {palette 0.75}

set view [get_charts_data -plot $plot1 -name view]

set_charts_property -view $view -name title.string -value "Growth in Internet Users Globally"

group_charts_plots -y1y2 -overlay $plot1 $plot2
