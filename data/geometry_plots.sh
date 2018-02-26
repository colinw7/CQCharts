CQChartsTest \
 -tsv states.tsv -comment_header \
 -type geometry -columns "name=0,geometry=1" \
 -plot_title "geometry" \
-and \
 -tsv choropeth.tsv \
 -type geometry -columns "name=0,geometry=1,value=2" \
 -plot_title "choropeth"

