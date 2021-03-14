set plots { }

### adjacency
set adjacency_model [load_charts_model -csv data/sankey.csv -comment_header \
  -column_type {{{0 name_pair}}}]

set adjacency_plot [create_charts_plot -model $adjacency_model -type adjacency \
  -columns {{link 0} {value 1}} -title "Adjacency"]

lappend plots $adjacency_plot

### barchart
#set barchart_model [load_charts_model -csv data/multi_bar.csv -first_line_header]
# 
#set barchart_plot [create_charts_plot -model $barchart_model -type barchart \
#  -columns {{name 0} {values 1}} -title "Barchart"]
#
#lappend plots $barchart_plot

### box
set box_model [load_charts_model -csv data/boxplot.csv -first_line_header]

set box_plot [create_charts_plot -model $box_model -type box \
  -columns {{group 0} {values 2}} -title "Box Plot" \
  -properties {{options.horizontal 0}}]

lappend plots $box_plot

### bubble
#set bubble_model [load_charts_model -csv data/flare1.csv -comment_header \
#  -column_type {{{2 real}}}]
#
#set bubble_plot [create_charts_plot -model $bubble_model -type bubble \
#  -columns {{name 1} {value 2}} -title "Bubble"]
#
#lappend plots $bubble_plot

### chord
#set chord_model [load_charts_model -csv data/sankey.csv -comment_header \
#  -column_type {{{0 name_pair}}}]
#
#set chord_plot [create_charts_plot -model $chord_model -type chord \
#  -columns {{link 0} {value 1}} -title "Chord"]
#
#lappend plots $chord_plot

### contour
#set contour_model [load_charts_model -csv data/contour.csv]
#
#set contour_plot [create_charts_plot -model $contour_model -type contour \
#  -title Contour]
#
#lappend plots $contour_plot

### correlation
#set correlation_model [load_charts_model -csv data/winequality-white.csv \
#  -first_line_header -separator {;}]
#
#set correlation_plot [create_charts_plot -type correlation -model $correlation_model \
#  -title "Wine Correlation"]
#
#lappend plots $correlation_plot

### delaunay
#set delaunay_model [load_charts_model -csv data/airports.csv -comment_header]

#set delaunay_plot [create_charts_plot -model $delaunay_model -type delaunay \
#  -columns {{x 6} {y 5} {name 1}} -title "Delaunay"]

#lappend plots $delaunay_plot

### dendrogram
#set dendrogram_model [load_charts_model -csv data/flare.csv -comment_header]
#
#set dendrogram_plot [create_charts_plot -model $dendrogram_model -type dendrogram \
#  -columns {{name 0} {value 1}} -title "Dendrogram"]
#
#lappend plots $dendrogram_plot

### distribution
#set distribution_model [load_charts_model -csv data/boxplot.csv -first_line_header \
# -column_type {integer integer integer}]
#
#set distribution_plot [create_charts_plot -type distribution -parameter {valueType RANGE} \
#  -columns {{values 0} {data 2}} -title "Distribution"]
#
#lappend plots $distribution_plot

### forceDirected
#set force_directed_model [load_charts_model -csv data/sankey_energy.csv -comment_header \
# -column_type {{{0 name_pair}}}]

#set force_directed_plot [create_charts_plot -model $force_directed_model -type force_directed \
#  -columns {{link 0} {value 1}} -title "Force Directed"]

#lappend plots $force_directed_plot

### geometry
#set geometry_model [load_charts_model -csv data/geom_boxes_1.csv -first_line_header \
#  -column_type {{{1 rectangle}}}]
#
#set geometry_plot [create_charts_plot -model $geometry_model -type geometry \
#  -columns {{name 0} {geometry 1}} -xmin 0 -ymin 0 -xmax 100 -ymax 100 \
#  -title "Geometry"]
#
#lappend plots $geometry_plot

### graph
#set graph_model [load_charts_model -csv data/dot_fsm.csv -first_line_header]

#set graph_plot [create_charts_plot -model $graph_model -type graph \
#  -columns {{from 0} {to 1} {attributes 2}} -title "Graph"]

#lappend plots $graph_plot

### grid

### hierbubble
#set hierbubble_model [load_charts_model -csv data/flare.csv -comment_header \
#  -column_type {{{1 real}}}]

#set hierbubble_plot [create_charts_plot -model $hierbubble_model -type hierbubble \
#  -columns {{name 0} {value 1}} -title "Hier Bubble"]

#lappend plots $hierbubble_plot

### hierscatter

### image

### parallel
#set parallel_model [load_charts_model -csv data/parallel_coords.csv -first_line_header]

#set parallel_plot [create_charts_plot -model $parallel_model -type parallel \
#  -columns {{x 0} {y {1 2 3 4 5 6 7}}} -title "Parallel"]

#lappend plots $parallel_plot

### pie
#set pie_model [load_charts_model -csv data/ages.csv -first_line_header \
#  -column_type {{{1 integer}}}]

#set pie_plot [create_charts_plot -model $pie_model -type pie \
#  -columns {{label 0} {values 1}} -title "Pie Chart"]

#lappend plots $pie_plot

### pivot

### radar
#set radar_model [load_charts_model -csv data/radar.csv -first_line_header]

#set radar_plot [create_charts_plot -model $radar_model -type radar \
#  -columns {{name 0} {values {1 2 3 4 5}}} -title "Radar Plot"]

#lappend plots $radar_plot

### sankey

### scatter

### scatter3d

### strip

### sunburst

### table

### treemap

### wheel

### wordCloud

### xy

group_charts_plots -composite -tabbed $plots
