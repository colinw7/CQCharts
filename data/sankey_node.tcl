# connections column model
set model [load_charts_model -tsv data/adjacency.tsv -comment_header \
 -column_type {{{3 connection_list}}}]

set plot [create_charts_plot -model $model -type sankey \
  -columns {{node 1} {connections 3} {name 0} {group 2}} -title "sankey"]

set_charts_property -plot $plot -name options.align -value SRC
