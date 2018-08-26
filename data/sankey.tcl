# Sankey

set model1 [load_model -csv data/sankey.csv -comment_header]

set plot1 [create_plot -model $model1 -type sankey -columns "link=0,value=1"]

#set model2 [load_model -csv data/sankey_energy.csv -comment_header]

#set plot2 [create_plot -model $model2 -type sankey -columns "link=0,value=1"]
