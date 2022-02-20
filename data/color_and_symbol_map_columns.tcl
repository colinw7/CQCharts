set symbol_data [list \
 [list Index Value Number Name] \
 [list 1 10 one   fred] \
 [list 2 20 two   bill] \
 [list 3 30 three harry] \
 [list 4 20 three one] \
 [list 5 15 four  two] \
 [list 6 25 two   three] \
 [list 7 35 two   two] \
 [list 8 20 one   harry] \
 [list 9 5  one   bill] \
]

set model [load_charts_model -var symbol_data -transpose -first_line_header]

#---

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x 0} {y 1} {color 2} {symbolType 2} {symbolSize 2}} \
  -title "Scatter Color Map"]

set_charts_property -plot $plot1 -name mapping.color.enabled -value 1
set_charts_property -plot $plot1 -name mapping.symbolType.enabled -value 1
set_charts_property -plot $plot1 -name mapping.symbolSize.enabled -value 1

set_charts_property -plot $plot1 -name mapping.color.color_map \
  -value {{one red} {two green} {three blue} {four cyan}}
set_charts_property -plot $plot1 -name mapping.symbolType.symbol_map \
  -value {{one circle} {two square} {three triangle} {four star}}
set_charts_property -plot $plot1 -name mapping.symbolSize.size_map \
  -value {{one 0.1P} {two 0.2P} {three 0.3P} {four 0.4P}}

set_charts_property -plot $plot1 -name mapKeys.color.visible -value 1
set_charts_property -plot $plot1 -name mapKeys.symbolType.visible -value 1
set_charts_property -plot $plot1 -name mapKeys.symbolSize.visible -value 1

set_charts_property -plot $plot1 -name points.size -value 12px
