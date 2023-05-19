# Dendrogram

set model [load_charts_model -json data/flare.json]

set plot [create_charts_plot -model $model -type dendrogram \
 -columns {{name 0} {value 1}} -title "dendrogram"]

set_charts_property -plot $plot -name options.followViewExpand -value 1

set_charts_property -plot $plot -name points.scaled -value 0

set_charts_property -plot $plot -name root.size -value 8px
set_charts_property -plot $plot -name hier.size -value 8px
set_charts_property -plot $plot -name leaf.size -value 8px
