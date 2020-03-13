set dose  { dose  20 30 40 45 60 }
set drugA { drugA 16 20 27 40 60 }
set drugB { drugB 15 18 25 31 40 }

set model [load_charts_model -tcl [list $dose $drugA $drugB] -first_line_header]

set plot1 [create_charts_plot -model $model -type xy -columns {{x dose} {y drugA}}]
set plot2 [create_charts_plot -model $model -type xy -columns {{x dose} {y drugB}}]

set_charts_property -plot $plot1 -name points.visible -value 1
set_charts_property -plot $plot2 -name points.visible -value 1

place_charts_plots -horizontal $plot1 $plot2
