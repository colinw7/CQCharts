set model [load_charts_model -json data/flare.json]

#set model [load_charts_model -csv data/sankey_energy.csv -comment_header \
  -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -type table -model $model -columns {{columns {0 1 2 3 4}}}]
