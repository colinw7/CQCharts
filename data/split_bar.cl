load -csv split_bar.csv -first_line_header
set model = $_rc

plot -type bar -columns "category=0,value=1"
set plot1 = $_rc

plot -type bar -columns "category=0,value=2"
set plot2 = $_rc

set view = "view1"

set_property -view $view -plot $plot1 -name rect -value {0 0 50 100}
set_property -view $view -plot $plot1 -name horizontal -value 1
set_property -view $view -plot $plot1 -name invertX -value 1
set_property -view $view -plot $plot1 -name invertY -value 1
set_property -view $view -plot $plot1 -name plotStyle.fill.visible -value 0
set_property -view $view -plot $plot1 -name dataStyle.fill.visible -value 0
set_property -view $view -plot $plot1 -name xaxis.side -value TOP_RIGHT
set_property -view $view -plot $plot1 -name xaxis.grid.line.major.visible -value 1
set_property -view $view -plot $plot1 -name xaxis.ticks.label.angle -value 30
set_property -view $view -plot $plot1 -name yaxis.label.visible -value 0
set_property -view $view -plot $plot1 -name key.visible -value 0
set_property -view $view -plot $plot1 -name title.text.text -value "Male"
set_property -view $view -plot $plot1 -name title.location -value bottom
set_property -view $view -plot $plot1 -name fill.color -value palette:0.5
set_property -view $view -plot $plot1 -name margin.right -value 5

set_property -view $view -plot $plot2 -name rect -value {50 0 100 100}
set_property -view $view -plot $plot2 -name horizontal -value 1
set_property -view $view -plot $plot2 -name invertY -value 1
set_property -view $view -plot $plot2 -name plotStyle.fill.visible -value 0
set_property -view $view -plot $plot2 -name dataStyle.fill.visible -value 0
set_property -view $view -plot $plot2 -name xaxis.side -value TOP_RIGHT
set_property -view $view -plot $plot2 -name xaxis.grid.line.major.visible -value 1
set_property -view $view -plot $plot2 -name xaxis.ticks.label.angle -value 30
set_property -view $view -plot $plot2 -name yaxis.label.visible -value 0
set_property -view $view -plot $plot2 -name yaxis.ticks.label.visible -value 0
set_property -view $view -plot $plot2 -name key.visible -value 0
set_property -view $view -plot $plot2 -name title.text.text -value "Female"
set_property -view $view -plot $plot2 -name title.location -value bottom
set_property -view $view -plot $plot2 -name fill.color -value palette:0.8
set_property -view $view -plot $plot2 -name margin.left -value 5
