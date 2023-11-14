# Dendrogram

set model [load_charts_model -csv data/dendrogram_all_size.csv -first_line_header]

set plot [create_charts_plot -model $model -type dendrogram \
 -columns {{name 0} {value 1}} -title "sorted dendrogram"]

set_charts_property -plot $plot -name options.placeType -value SORTED

set_charts_property -plot $plot -name root.visible -value 0

set_charts_property -plot $plot -name root.size -value 0.05P
set_charts_property -plot $plot -name hier.size -value 0.05P
set_charts_property -plot $plot -name leaf.size -value 0.05P

set_charts_property -plot $plot -name root.shape -value rect
set_charts_property -plot $plot -name hier.shape -value rect
set_charts_property -plot $plot -name leaf.shape -value rect

set_charts_property -plot $plot -name root.label.valueLabel -value 1
set_charts_property -plot $plot -name hier.label.valueLabel -value 1
set_charts_property -plot $plot -name leaf.label.valueLabel -value 1

set_charts_property -plot $plot -name root.label.scaled -value 1
set_charts_property -plot $plot -name hier.label.scaled -value 1
set_charts_property -plot $plot -name leaf.label.scaled -value 1

set_charts_property -plot $plot -name root.label.position -value CENTER
set_charts_property -plot $plot -name hier.label.position -value CENTER
set_charts_property -plot $plot -name leaf.label.position -value CENTER
