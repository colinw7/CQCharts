# Dendrogram (edge values)

set model [load_charts_model -csv data/sankey.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type dendrogram \
  -columns {{link 0} {value 1}} -title "dendrogram"]

set_charts_property -plot $plot -name root.size -value 0.1P
set_charts_property -plot $plot -name hier.size -value 0.1P
set_charts_property -plot $plot -name leaf.size -value 0.1P

set_charts_property -plot $plot -name edge.sizeByValue -value 1
set_charts_property -plot $plot -name edge.width       -value 0.1P
