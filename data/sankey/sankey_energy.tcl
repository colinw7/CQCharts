# sankey link model
set model [load_charts_model -csv data/sankey_energy.csv -comment_header \
 -column_type {{{0 name_pair}}}]

set plot [create_charts_plot -model $model -type sankey -columns {{link 0} {value 1}}]

set_charts_property -plot $plot -name title.text.string    -value "Energy Distribution"
set_charts_property -plot $plot -name dataBox.clip         -value 0
set_charts_property -plot $plot -name dataBox.fill.visible -value 0
