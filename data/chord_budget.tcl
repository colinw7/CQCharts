set model [load_charts_model -csv data/sankey.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type chord -columns {{link 0} {value 1}}]

set_charts_property -plot $plot -name options.symmetric -value 0
