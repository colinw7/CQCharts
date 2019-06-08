set model [load_charts_model -csv data/pareto.csv -comment_header]

process_charts_model -add -header Sum -expr "column(1)+cell(@r-1,@c,0)" -type {real}
process_charts_model -add -header Percent -expr "100.0*column(1)/cell(@nr-1,2)" \
  -type {real {format %g%%}}

set nr [get_charts_data -model $model -name num_rows]

set max [get_charts_data -model $model -column 2 -row [expr {$nr - 1}] -name value]

set view [create_charts_view]

set_charts_property -view $view -name inside.highlight.mode -value FILL
set_charts_property -view $view -name inside.highlight.fill.color -value red

set plot1 [create_charts_plot -model $model -type bar -columns {{name 0} {value 1}} -ymax $max]

set_charts_data -plot $plot1 -name updates_enabled -value 0

set view [get_charts_property -plot $plot1 -name viewId]

set_charts_property -view $view -name title.string -value "Popular Fast Food Chains"

set_charts_property -plot $plot1 -name "yaxis.grid.lines" -value MAJOR

set plot2 [create_charts_plot -model $model -type xy -columns {{x 0} {y 3}} -ymin 0]

set_charts_data -plot $plot2 -name updates_enabled -value 0

set_charts_property -plot $plot2 -name cumulative.enabled -value 1

set_charts_property -plot $plot2 -name points.visible             -value 1
set_charts_property -plot $plot2 -name points.symbol.type         -value circle
set_charts_property -plot $plot2 -name points.symbol.size         -value 6px
set_charts_property -plot $plot2 -name points.symbol.fill.visible -value 1

set_charts_property -plot $plot2 -name lines.stroke.width -value 3px

group_charts_plots -y1y2 -overlay $plot1 $plot2

set_charts_data -plot $plot1 -name updates_enabled -value 1
set_charts_data -plot $plot2 -name updates_enabled -value 1

set_charts_property -plot $plot1 -name margins.inner.left   -value 8px
set_charts_property -plot $plot1 -name margins.inner.right  -value 8px
set_charts_property -plot $plot1 -name margins.inner.bottom -value 8px
set_charts_property -plot $plot1 -name margins.inner.top    -value 8px
