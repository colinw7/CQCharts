set model [load_model -csv data/USArrests.csv -first_line_header -first_column_header]

process_model -model $model -add -expr "scale(#{Murder})" -header "Murder (Scaled)"
process_model -model $model -add -expr "scale(#{Assault})" -header "Assault (Scaled)"
process_model -model $model -add -expr "scale(#{UrbanPop})" -header "UrbanPop (Scaled)"
process_model -model $model -add -expr "scale(#{Rape})" -header "Rape (Scaled)"

set scaleModel [create_subset_model -model $model -left 4 -right 7]

set corrModel [create_correlation_model -model $scaleModel -flip]

set tranposeModel [create_transpose_model -model $scaleModel]

set plot [create_plot -model $corrModel -type image]

set_charts_property -plot $plot -name labels.x.visible -value 1
set_charts_property -plot $plot -name labels.y.visible -value 1

manage_model_dlg

#set plot [create_plot -type xy -columns "group=0,x=1,y=2" -title "Group XY Plot"]
