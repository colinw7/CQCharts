# Dendrogram

CQChartsTest -csv data/flare.csv -comment_header \
 -type dendrogram -columns "name=0,value=1" \
 -plot_title "dendrogram"
CQChartsTest -json data/flare.json \
 -type dendrogram -columns "name=0,value=1" \
 -plot_title "dendrogram"
