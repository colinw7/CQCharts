# Dendrogram

set model [load_charts_model -json data/flare.json]

set plot [create_charts_plot -model $model -type dendrogram \
 -columns {{name 0} {value 1} {color 1} {size 1}} -title "dendrogram"]

set_charts_property -plot $plot -name options.followViewExpand -value 1

set_charts_property -plot $plot -name node.symbolSize -value 0.1P
set_charts_property -plot $plot -name node.symbolAspect -value 2

set_charts_property -plot $plot -name root.symbol -value box
set_charts_property -plot $plot -name root.label.position -value CENTER
#set_charts_property -plot $plot -name root.label.scaled -value 1

set_charts_property -plot $plot -name hier.symbol -value box
set_charts_property -plot $plot -name hier.label.position -value CENTER
set_charts_property -plot $plot -name hier.label.scaled -value 1
set_charts_property -plot $plot -name hier.label.font -value bold

set_charts_property -plot $plot -name leaf.symbol -value box
set_charts_property -plot $plot -name leaf.label.position -value CENTER
set_charts_property -plot $plot -name leaf.label.scaled -value 1

test_charts -type adjust_rects
