set model [load_model -csv data/pareto.csv -comment_header]

process_model -add -header Sum "column(1)+cell(@r-1,@c,0)" -type "real"
process_model -add -header Percent "100.0*column(1)/cell(@nr-1,2)" -type "real:format=%g%%"

set nr [get_property -model $model -name num_rows]

set max [get_model -ind $model -column 2 -row [expr {$nr - 1}]]

set plot1 [create_plot -type bar -columns "name=0,value=1" -ymax $max]

set view [get_property -plot $plot1 -name viewId]

set_property -view $view -name title -value "Popular Fast Food Chains"

set_property -view $view -plot $plot1 -name "yaxis.grid.line.major.visible" -value 1

set plot2 [create_plot -type xy -columns "x=0,y=3" -ymin 0]

set_property -view $view -plot $plot2 -name cumulative.enabled  -value 1
set_property -view $view -plot $plot2 -name points.symbol       -value circle
set_property -view $view -plot $plot2 -name points.size         -value 6
set_property -view $view -plot $plot2 -name points.fill.visible -value 1
set_property -view $view -plot $plot2 -name lines.width         -value 3px

group_plots -y1y2 -overlay $plot1 $plot2
