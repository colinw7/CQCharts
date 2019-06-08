set model [load_charts_model -csv data/HairEyeColor.csv -first_line_header]

set plot1 [create_charts_plot -model $model -type scatter \
  -columns {{x Hair} {y Eye} {symbolSize Freq}} \
  -where {$Sex=="Male"} -title "Male"]
set plot2 [create_charts_plot -model $model -type scatter \
  -columns {{x Hair} {y Eye} {symbolSize Freq}} \
  -where {$Sex=="Female"} -title "Female"]

place_charts_plots -horizontal [list $plot1 $plot2]

#show_charts_manage_models_dlg
