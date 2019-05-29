# Dendrogram

set model1 [load_charts_model -csv data/flare.csv -comment_header]

set plot1 [create_charts_plot -model $model1 -type dendrogram -columns {{name 0} {value 1}} -title "dendrogram"]

#set model2 [load_charts_model -json data/flare.json]

#set plot2 [create_charts_plot -model $model2 -type dendrogram -columns {{name 0} {value 1}} -title "dendrogram"]
