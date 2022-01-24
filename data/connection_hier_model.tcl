#set model1 [load_charts_model -csv data/sankey_energy.csv -comment_header \
# -column_type {{{0 name_pair}}}]
#set hierModel1 [connection_charts_model -model $model1]

#set model2 [load_charts_model -tsv data/adjacency.tsv -comment_header \
# -column_type {{{3 connection_list}}}]
#set hierModel2 [connection_charts_model -model $model2 -node 1 -connections 3]

set model3 [load_charts_model -csv data/connectivity.csv -first_line_header \
 -column_type {{{2 connection_list}}}]
set hierModel3 [connection_charts_model -model $model3 -node 1 -connections 2]

#set plot [create_charts_plot -model $hierModel1 -type dendrogram -columns {{name 0} {value 1}}]
#set plot [create_charts_plot -model $hierModel2 -type dendrogram -columns {{name 0} {value 1}}]
set plot [create_charts_plot -model $hierModel3 -type dendrogram -columns {{name 0} {value 1}}]
