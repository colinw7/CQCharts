# Sankey

#set model1 [load_charts_model -csv data/sankey.csv -comment_header \
# -column_type {{{0 name_pair}}}]

#set plot1 [create_charts_plot -model $model1 -type sankey -columns {{link 0} {value 1}}]

set model2 [load_charts_model -csv data/sankey_energy.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot2 [create_charts_plot -model $model2 -type sankey -columns {{link 0} {value 1}}]
