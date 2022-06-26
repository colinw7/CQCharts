Per Plot
--------

column       Description/Plots
------       -----------------
x            Values for X Axis (2D Plots)
               box, delaunay, hier_scatter, parallel, scatter, xy
y            Value for Y Axis (2D Plots)
               delaunay, hier_scatter, parallel, scatter, xy
value        Value for Y Axis (1D Plots - Directional)
               bar, box, bubble, chord, dendogram, geometry, hier, pie, radar, sankey
name         Name for Value Groups (1D Plots) and Individual Values (2D Plots)
               adjacency, barchart, box, bubble, delaunay, dendogram, distribution, force_directed,
               geometry, hier, hier_scatter, radar, scatter
label        Label for Objects (1D Plots)
               barchart, pie (TODO: difference between name and label is obscure/needed)
color        Color for Object create from Row
               All plots
font         Font for Text of Object create from Row
               All plots
symbolType   Symbol Type for Point created from Row
               scatter, xy
symbolSize   Symbol Size for Point created from Row
               scatter, xy
image        Image for Object create from Row
               all plots ?

group        Grouping Value for Grouped Plots
               chord, group, hier_scatter

link         from/to edge data
              connection plots (adjacency, chord, force_directed, graph, graphviz, sankey)
node         node index (use with connections)
              connection plots
connections  list of from/to indices (use with node)
              connection plots
count        word cloud

data         distribution
set          box
min          box
lowerMedian  box
median       box
upperMedian  box
max          box
outliers     box
geometry     geometry
style        geometry
radius       pie
keyLabel     pie
id           custom id for object (for scripting)
              plot
tip          plot
visible      plot
vectorX      xy
vectorY      xy
pointLabel   xy

Usage
-----

Column  Usage
------  -----
x       2D x value
y       2D y value
value   1D value
name    value name (also used for grouping of values with same name)
label   value name for label (not used for grouping)
color   value color
image   value image
