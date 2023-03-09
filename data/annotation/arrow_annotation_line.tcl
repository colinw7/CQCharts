set modelId [load_charts_model -csv data/arrowstyle.csv]

set plotId [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 100 -ymax 100]

set arrowId [create_charts_arrow_annotation -plot $plotId -start {10 10} -end {90 90}]

set_charts_property -annotation $arrowId -name fill.visible -value 0
set_charts_property -annotation $arrowId -name fill.color -value white
set_charts_property -annotation $arrowId -name stroke.visible -value 1
set_charts_property -annotation $arrowId -name stroke.color -value black
set_charts_property -annotation $arrowId -name line.width -value 5P
set_charts_property -annotation $arrowId -name midHead.type -value LINE
set_charts_property -annotation $arrowId -name midHead.length -value 10P

#set_charts_property -annotation $arrowId -name debugLabels -value 1
