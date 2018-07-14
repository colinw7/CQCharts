load_model -csv data/pareto.csv -comment_header
model = _rc

process_model -add -header Sum -expr "cell(@r,1)+cell(@r-1,@c,0)" -type "real"
process_model -add -header Percent -expr "100.0*column(1)/cell(@nr-1,2)" -type "real:format=%g%%"

get_charts_data -model $model -name num_rows
nr = _rc

get_charts_data -model $model -column 2 -row [expr {$nr - 1}] -name value
max = _rc

create_plot -type bar -columns "name=0,value=1" -ymax $max
plot1 = _rc

get_property -plot $plot1 -name viewId
view = _rc

set_property -view $view -name title -value "Popular Fast Food Chains"

set_property -plot $plot1 -name "yaxis.grid.line.major.visible" -value 1

create_plot -type xy -columns "x=0,y=3" -ymin 0
plot2 = _rc

set_property -plot $plot2 -name cumulative.enabled  -value 1
set_property -plot $plot2 -name points.symbol       -value circle
set_property -plot $plot2 -name points.size         -value 6
set_property -plot $plot2 -name points.fill.visible -value 1
set_property -plot $plot2 -name lines.width         -value 3px

group_plots -y1y2 -overlay $plot1 $plot2
