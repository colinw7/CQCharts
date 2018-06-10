set modelId [load_model -csv data/aster_data.csv -first_line_header]

set nr [get_property -model $modelId -name num_rows]

set total 0.0

for {set i 0} {$i < $nr} {incr i} {
  set score [get_charts_data -model $modelId -row $i -column 2 -name value]

  set weight [get_charts_data -model $modelId -row $i -column 3 -name value]

  set total [expr {$total + $score*$weight}]
}

set average [expr {$total/$nr}]

set_charts_data -model $modelId -name column_type -value "4#color"

set plotId [create_plot -type pie -columns "id=0,radius=2,data=3,label=5,color=4" -title "Aster"]

set viewId [get_property -plot $plotId -name viewId]

set_property -plot $plotId -name donut -value 1
set_property -plot $plotId -name grid.visible -value 1
set_property -plot $plotId -name innerRadius -value 0.2

set averageStr [format "%.0f" $average]

set textId [create_text_shape -plot $plotId -x 0 -y 0 -text $averageStr -align aligncenter]

#set_property -annotation $textId -name textAlign -value aligncenter
