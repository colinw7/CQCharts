set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set arrowId [create_charts_arrow_annotation -plot $plotId -start {10 10} -end {90 90}]
 
#set_charts_property -annotation $arrowId -name path -value {M 10 10 L 90 90}
set_charts_property -annotation $arrowId -name path -value {M 10 10 C 10 50 50 90 90 90 }
