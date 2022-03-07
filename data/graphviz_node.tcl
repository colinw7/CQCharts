set ::env(CQ_CHARTS_GRAPHVIZ_INPUT_FILE) /tmp/dot_in.gv
set ::env(CQ_CHARTS_GRAPHVIZ_OUTPUT_FILE) /tmp/dot_out.dot

# connections column model
set model [load_charts_model -tsv data/adjacency.tsv -comment_header \
 -column_type {{{3 connection_list}}}]

set plot [create_charts_plot -model $model -type graphviz \
  -columns {{node 1} {connections 3} {name 0} {group 2}} -title "adjacency"]
