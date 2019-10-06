# Bubble/Hier Bubble Plot

CQChartsTest -json data/flare.json \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"
CQChartsTest -csv data/flare.csv -comment_header \
 -type bubble -columns "name=0,value=1" -column_type "1#real" \
 -plot_title "bubble"
CQChartsTest -csv data/flare.csv -comment_header \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"
CQChartsTest -csv data/pareto.csv -comment_header \
 -type bubble -columns "name=0,value=1" \
 -plot_title "bubble"

CQChartsTest -json data/flare.json \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble (hier data)"
CQChartsTest -csv data/flare.csv -comment_header \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble (flat data)"
CQChartsTest -csv data/hier.csv -comment_header \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hierarchical bubble"
CQChartsTest -csv data/hier_files.csv \
 -type hierbubble -columns "name=0,value=1" \
 -plot_title "hier files"
CQChartsTest -tsv data/coffee.tsv -first_line_header \
 -type hierbubble -columns "name=0,color=1" \
 -plot_title "coffee characteristics"

CQChartsTest -csv data/hier_order.csv -comment_header \
 -type hierbubble -columns "name=0,value=1,color=2" \
 -plot_title "hierarchical bubble"

CQChartsTest -csv data/book_revenue.csv -first_line_header \
 -type hierbubble -columns "names=0 1 2,value=3" \
 -plot_title "book revenue"
