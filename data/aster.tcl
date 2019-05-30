set modelId [load_charts_model -csv data/aster_data.csv -first_line_header]

set nr [get_charts_data -model $modelId -name num_rows]

set total 0.0

for {set i 0} {$i < $nr} {incr i} {
  set score [get_charts_data -model $modelId -row $i -column 2 -name value]

  set weight [get_charts_data -model $modelId -row $i -column 3 -name value]

  set total [expr {$total + $score*$weight}]
}

set average [expr {$total/$nr}]

set_charts_data -model $modelId -name column_type -column 4 -value {{color}}

set plotId [create_charts_plot -type pie -columns {{id 0} {radius 2} {value 3} {label 5} {color 4}} -title "Aster"]

set viewId [get_charts_property -plot $plotId -name viewId]

set_charts_property -plot $plotId -name options.donut       -value 1
set_charts_property -plot $plotId -name grid.visible        -value 1
set_charts_property -plot $plotId -name options.innerRadius -value 0.2

set averageStr [format "%.0f" $average]

set textId [create_charts_text_annotation -plot $plotId -position {0 0} -text $averageStr -align aligncenter]

#set_charts_property -annotation $textId -name textAlign -value aligncenter
