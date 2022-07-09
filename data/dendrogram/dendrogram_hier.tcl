# Dendrogram

set model [load_charts_model -json data/flare.json]

set plot [create_charts_plot -model $model -type dendrogram \
 -columns {{name 0} {value 1}} -title "dendrogram"]

set_charts_property -plot $plot -name options.followViewExpand -value 1

set_charts_property -plot $plot -name points.scaled -value 0
set_charts_property -plot $plot -name node.symbolSize -value 8px
