1D : value set
 barchart, box, bubble, distribution, hierbubble, pie, radar (multiple), strip, treemap, wordcloud
2D : point set
 contour (3D ?), delaunay, hierscatter, parallel (multiple), pivot, scatter, wheel, xy

Connectivity ?
  adjacency, chord, force directed, graph, sankey

Hier ?
  dendrogram ?, hier bubble, sunburst, treemap

adjacency     : rectangle                : annotation_adjacency_plot.tcl
barchart      : rectangle, value set     : annotation_bar_chart_group.tcl
box           : value set (density ?)    : annotation_boxplot.tcl
bubble        : ellipse + pack           : annotation_bubble_plot.tcl
chord         :                          : annotation_chord_plot.tcl
contour       : point3d set (contour)    : point3d_set_annotation.tcl
correlation   :                          : TODO
delaunay      : point set (delaunay)     : point_set_annotation.tcl
dendrogram    :                          : TODO
distribution  : rectangle                : annotation_dist_chart_group.tcl
forcedirected :                          : TODO
geometry      : point, polygon, (more ?) :
graph         :                          : annotation_budget_graph.tcl
grid          :                          :
hierbubble    : ellipse + pack ?         : annotation_hier_bubble.tcl
hierscatter   : point set (symbols) ?    :
image         : rectangle, ...           :
parallel      : axis + pack ?            : annotation_parallel_plot.tcl
pie           : value set (pie)          :
pivot         :                          : pie_slice_annotation.tcl
radar         : polygon ?                : annotation_radar_plot.tcl
sankey        :                          : (same as graph ?)
scatter3D     : point3d set (symbols)    :
scatter       : point set (symbols)      : annotation_scatter.tcl
strip         : point ?                  :
sunburst      : arc ?                    : annotation_sunburst_plot.tcl
table         :                          :
treemap       : value set (treemap)      : annotation_treemap.tcl
wheel         :                          :
wordcloud     :                          : text_cloud_annotation_place.tcl
xy            : polygon                  : annotation_xy_plot.tcl
