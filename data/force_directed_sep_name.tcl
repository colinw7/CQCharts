# Adjacency

set model [load_charts_model -csv data/sankey_energy.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type forcedirected -columns {{namePair 0} {count 1}}]
