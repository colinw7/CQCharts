set model [load_charts_model -csv data/sankey.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type sankey -columns {{link 0} {value 1}}]
