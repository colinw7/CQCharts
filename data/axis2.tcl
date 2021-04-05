set x {1 2 3 4 5 6 7 8 9 10}
set y $x

set z {}

foreach xv $x {
  lappend z [expr {10.0/$xv}]
}

set model [load_charts_model -tcl [list $x $y $z]]

set view [create_charts_view]

set plot1 [create_charts_plot -model $model -view $view -type xy -columns {{x 0} {y 1}}]
set plot2 [create_charts_plot -model $model -view $view -type xy -columns {{x 0} {y 2}}]

set_charts_property -plot $plot1 -name points.visible -value 1
set_charts_property -plot $plot2 -name points.visible -value 1

set_charts_property -plot $plot1 -name xaxis.label.text.string -value "X values"
set_charts_property -plot $plot1 -name yaxis.label.text.string -value "Y=X"

set_charts_property -plot $plot2 -name yaxis.customTickLabels -value $z
set_charts_property -plot $plot2 -name yaxis.label.text.string -value "y=1/x"

set_charts_property -view $view -name title.string -value "An Example of Creative Axes"

group_charts_plots -y1y2 -overlay $plot1 $plot2
