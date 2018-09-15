# Sunburst Plot

CQChartsTest -json data/flare.json \
 -type sunburst -columns "names=0,value=1" \
 -plot_title "sunburst"
CQChartsTest -csv data/flare.csv -comment_header \
 -type sunburst -columns "names=0,value=1" \
 -plot_title "sunburst"
CQChartsTest -tsv data/coffee.tsv -first_line_header \
 -type sunburst -columns "names=0,color=1" \
 -plot_title "coffee characteristics" \
 -properties "multiRoot=1"
CQChartsTest -csv data/book_revenue.csv -first_line_header \
 -type sunburst -columns "names=0 1 2,value=3" \
 -plot_title "book revenue"
