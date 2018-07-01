#set model [load_model -csv data/group.csv -comment_header]
#set plot [create_plot -type bubble -columns "group=1,value=2" -title "Bubble Group"]

set model [load_model -csv data/hier.csv -comment_header]
set plot [create_plot -type bubble -columns "group=0,value=1" -title "Bubble Group"]

#set model [load_model -csv data/flare.csv -comment_header]


create_plot_dlg -model $model
