set model [load_model -csv data/HairEyeColor.csv -first_line_header]

set plot1 [create_plot -model $model -type barchart -columns "group=Hair,value=Freq,color=Eye" -where {$Sex=="Male"} -title "Male"]
set plot2 [create_plot -model $model -type barchart -columns "group=Hair,value=Freq,color=Eye" -where {$Sex=="Female"} -title "Female"]

place_plots -horizontal [list $plot1 $plot2]

#manage_model_dlg
