set model [load_charts_model -csv data/USArrests.csv -first_line_header -first_column_header]

process_charts_model -model $model -add -expr "scale(#{Murder})" -header "Murder (Scaled)"
process_charts_model -model $model -add -expr "scale(#{Assault})" -header "Assault (Scaled)"
process_charts_model -model $model -add -expr "scale(#{UrbanPop})" -header "UrbanPop (Scaled)"
process_charts_model -model $model -add -expr "scale(#{Rape})" -header "Rape (Scaled)"

set scaleModel [create_charts_subset_model -model $model -left 4 -right 7]

set corrModel [create_charts_correlation_model -model $scaleModel -flip]

set tranposeModel [create_charts_transpose_model -model $scaleModel]

set plot [create_charts_plot -model $corrModel -type image]

set_charts_property -plot $plot -name xaxis.text.visible -value 1
set_charts_property -plot $plot -name yaxis.text.visible -value 1

show_charts_manage_models_dlg

#set plot [create_charts_plot -type xy -columns {{group 0} {x 1} {y 2}} -title "Group XY Plot"]
