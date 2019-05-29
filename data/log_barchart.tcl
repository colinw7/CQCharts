set model [load_charts_model -csv data/log.csv -comment_header]

set plot1 [create_charts_plot -type bar -columns {{name 0} {value 1}} -title "Log Scale"]
#set plot2 [create_charts_plot -type bar -columns {{x 0} {y 1}} -title "Linear Scale"]

set_charts_property -plot $plot1 -name "yaxis.valueType" -value "LOG"

set view [get_charts_data -plot $plot1 -name view]

set_charts_property -view $view -name title.string -value "Growth in Internet Users Globally"

#group_charts_plots -y1y2 -overlay $plot1 $plot2
