# Bar chart key click should hide all

set model [load_charts_model -tsv data/monthly_sales.tsv -comment_header \
  -column_type {{{1 real} {format $%gK} {format_scale 0.001}} \
                {{2 real} {format $%gK} {format_scale 0.001}} \
                {{3 real} {format $%gK} {format_scale 0.001}}}]

set plot1 [create_charts_plot -type barchart -columns {{group 0} {value 1} {label 4}} -title "Actual Sales" -ymax 80000]

set view [get_charts_property -plot $plot1 -name viewId]

set_charts_property -view $view -name title.string -value "Monthly Sales Data"

###---

set_charts_palette -theme default -name palettes -value [list default palette1 palette2 set2 set1 set3 plasma chromajs distinct1 distinct2]

set_charts_palette -palette default -name defined_colors -value {{0 "#6d78ad"} {1 "#51cda0"} {2 "#df7970"}}
#set_charts_palette -palette default -name defined_colors -value {{0 "#4d81bc"} {1 "#c0504e"} {2 "#9bbb58"}}

#set_charts_palette -interface -name defined_colors {{0 #ffffff} {1 #000000} {2 #808080}}
#set_charts_palette -interface -name defined_colors {{0 #2a2a2a} {1 #ffffff} {2 #808080}}

set colors [get_charts_palette -palette default -name defined_colors]
puts [lindex [lindex $colors 0] 1]
puts [lindex [lindex $colors 1] 1]
puts [lindex [lindex $colors 2] 1]

###---

set_charts_property -view $view -name inside.highlight.mode -value OUTLINE

set_charts_property -plot $plot1 -name margins.bar                -value 8px
#set_charts_property -plot $plot1 -name fill.color                 -value {palette 0 s}
set_charts_property -plot $plot1 -name coloring.colorBySet        -value 1
set_charts_property -plot $plot1 -name coloring.type              -value SET
set_charts_property -plot $plot1 -name labels.visible             -value 1
set_charts_property -plot $plot1 -name labels.position            -value TOP_OUTSIDE
#set_charts_property -plot $plot1 -name labels.color               -value {interface 1}
#set_charts_property -plot $plot1 -name plotStyle.fill.color       -value {interface 0}
#set_charts_property -plot $plot1 -name dataStyle.fill.color       -value {interface 0}
set_charts_property -plot $plot1 -name "xaxis.majorIncrement"     -value 1
set_charts_property -plot $plot1 -name "xaxis.label.text.string"  -value ""
#set_charts_property -plot $plot1 -name "xaxis.label.color"        -value {interface 1}
#set_charts_property -plot $plot1 -name "xaxis.ticks.label.color"  -value {interface 1}
set_charts_property -plot $plot1 -name "yaxis.grid.lines"         -value MAJOR
#set_charts_property -plot $plot1 -name "yaxis.grid.major.stroke.color"   -value {interface 1}
#set_charts_property -plot $plot1 -name "yaxis.label.color"        -value {interface 1}
#set_charts_property -plot $plot1 -name "yaxis.ticks.label.color"  -value {interface 1}
#set_charts_property -plot $plot1 -name key.visible                -value 0
set_charts_property -plot $plot1 -name key.insideY                -value 1
set_charts_property -plot $plot1 -name key.location               -value bc
set_charts_property -plot $plot1 -name key.horizontal             -value 1
set_charts_property -plot $plot1 -name key.fill.visible           -value 0
set_charts_property -plot $plot1 -name key.stroke.visible         -value 0
#set_charts_property -plot $plot1 -name key.text.color             -value {interface 1}
set_charts_property -plot $plot1 -name title.text.font            -value "FreeSans,18,-1,5,63,0,0,0,0,0"
#set_charts_property -plot $plot1 -name title.text.color           -value {interface 1}

###---

set plot2 [create_charts_plot -type xy -columns {{x 0} {y 2}} -title "Expected Sales"]

set_charts_property -plot $plot2 -name points.symbol.type           -value circle
set_charts_property -plot $plot2 -name points.symbol.size           -value 6px
set_charts_property -plot $plot2 -name points.symbol.fill.visible   -value 1
set_charts_property -plot $plot2 -name points.symbol.fill.color     -value {palette 1 s}
set_charts_property -plot $plot2 -name points.symbol.stroke.visible -value 0

set_charts_property -plot $plot2 -name lines.stroke.width -value 3px
set_charts_property -plot $plot2 -name lines.rounded      -value 1
set_charts_property -plot $plot2 -name lines.stroke.color -value {palette 1 s}

###---

set plot3 [create_charts_plot -type xy -columns {{x 0} {y 3}} -title "Profit"]

set_charts_property -plot $plot3 -name fillUnder.visible    -value 1
set_charts_property -plot $plot3 -name fillUnder.fill.color -value {palette 2 s}

set_charts_property -plot $plot3 -name points.symbol.type         -value circle
set_charts_property -plot $plot3 -name points.symbol.size         -value 6px
set_charts_property -plot $plot3 -name points.symbol.stroke.color -value white
set_charts_property -plot $plot3 -name points.symbol.stroke.width -value 3px
set_charts_property -plot $plot3 -name points.symbol.fill.visible -value 1
set_charts_property -plot $plot3 -name points.symbol.fill.color   -value {palette 2 s}
set_charts_property -plot $plot3 -name lines.rounded              -value 1

###---

group_charts_plots -overlay $plot1 $plot2 $plot3
