# Bar chart key click should hide all

set model [load_model -tsv data/monthly_sales.tsv -comment_header]

set plot1 [create_plot -type barchart -columns "category=0,value=1,label=4" -column_type "1#real:format=$%gK,format_scale=0.001;2#real:format=$%gK,format_scale=0.001;3#real:format=$%gK,format_scale=0.001" -title "Actual Sales" -ymax 80000]

set view [get_property -plot $plot1 -name viewId]

set_property -view $view -name title -value "Monthly Sales Data"

set_palette -palette 0 -color_type defined -defined "0=#6d78ad 1=#51cda0 2=#df7970"
#set_palette -palette 0 -color_type defined -defined "0=#4d81bc 1=#c0504e 2=#9bbb58"

#set_palette -interface -color_type defined -defined "0=#ffffff 1=#000000 2=#808080"
#set_palette -interface -color_type defined -defined "0=#2a2a2a 1=#ffffff 2=#808080"

set color0 [get_palette -get_color 0 -get_color_scale]
puts $color0
set color1 [get_palette -get_color 1 -get_color_scale]
puts $color1
set color2 [get_palette -get_color 2 -get_color_scale]
puts $color2

set_property -view $view -name insideHighlight.mode -value OUTLINE

set_property -view $view -plot $plot1 -name options.barMargin          -value 8px
set_property -view $view -plot $plot1 -name fill.color                 -value "palette:0:s"
set_property -view $view -plot $plot1 -name colorBySet                 -value 1
set_property -view $view -plot $plot1 -name dataLabel.visible          -value 1
set_property -view $view -plot $plot1 -name dataLabel.position         -value TOP_OUTSIDE
#set_property -view $view -plot $plot1 -name dataLabel.color            -value "interface:1"
#set_property -view $view -plot $plot1 -name plotStyle.fill.color       -value "interface:0"
#set_property -view $view -plot $plot1 -name dataStyle.fill.color       -value "interface:0"
set_property -view $view -plot $plot1 -name "xaxis.majorIncrement"    -value 1
set_property -view $view -plot $plot1 -name "xaxis.label.text"        -value ""
#set_property -view $view -plot $plot1 -name "xaxis.label.color"       -value "interface:1"
#set_property -view $view -plot $plot1 -name "xaxis.ticks.label.color" -value "interface:1"
set_property -view $view -plot $plot1 -name "yaxis.grid.line.major.visible" -value 1
#set_property -view $view -plot $plot1 -name "yaxis.grid.line.major.color"   -value "interface:1"
#set_property -view $view -plot $plot1 -name "yaxis.label.color"       -value "interface:1"
#set_property -view $view -plot $plot1 -name "yaxis.ticks.label.color" -value "interface:1"
#set_property -view $view -plot $plot1 -name key.visible                -value 0
set_property -view $view -plot $plot1 -name key.insideY                -value 0
set_property -view $view -plot $plot1 -name key.location               -value bc
set_property -view $view -plot $plot1 -name key.horizontal             -value 1
set_property -view $view -plot $plot1 -name key.background.visible     -value 0
set_property -view $view -plot $plot1 -name key.border.visible         -value 0
#set_property -view $view -plot $plot1 -name key.text.color             -value "interface:1"
set_property -view $view -plot $plot1 -name title.text.font            -value "FreeSans,18,-1,5,63,0,0,0,0,0"
#set_property -view $view -plot $plot1 -name title.text.color           -value "interface:1"

set plot2 [create_plot -type xy -columns "x=0,y=2" -title "Expected Sales"]

set_property -view $view -plot $plot2 -name points.symbol         -value circle
set_property -view $view -plot $plot2 -name points.size           -value 6
set_property -view $view -plot $plot2 -name points.fill.visible   -value 1
set_property -view $view -plot $plot2 -name points.fill.color     -value "palette:1:s"
set_property -view $view -plot $plot2 -name points.stroke.visible -value 0
set_property -view $view -plot $plot2 -name lines.width           -value 3px
set_property -view $view -plot $plot2 -name lines.rounded         -value 1
set_property -view $view -plot $plot2 -name lines.color           -value "palette:1:s"

set plot3 [create_plot -type xy -columns "x=0,y=3" -title "Profit"]

set_property -view $view -plot $plot3 -name fillUnder.visible   -value 1
set_property -view $view -plot $plot3 -name fillUnder.color     -value "palette:2:s"
set_property -view $view -plot $plot3 -name points.symbol       -value circle
set_property -view $view -plot $plot3 -name points.size         -value 6
set_property -view $view -plot $plot3 -name points.stroke.color -value white
set_property -view $view -plot $plot3 -name points.stroke.width -value 3px
set_property -view $view -plot $plot3 -name points.fill.visible -value 1
set_property -view $view -plot $plot3 -name points.fill.color   -value "palette:2:s"
set_property -view $view -plot $plot3 -name lines.rounded       -value 1

group_plots -overlay $plot1 $plot2 $plot3
