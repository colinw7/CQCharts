Per Plot
--------

column       Plots
------       -----
x            box, delaunay, hier_scatter, parallel, scatter, xy
y            delaunay, hier_scatter, parallel, scatter, xy
value        bar, box, bubble, chord, dendogram, geometry, hier, pie, radar, sankey
name         adjacency, barchart, box, bubble, delaunay, dendogram, distribution, force_directed,
             geometry, hier, hier_scatter, radar, scatter
label        barchart, pie
color        plot
symbolType   scatter, xy
symbolSize   scatter, xy
image        plot

group        chord, group, hier_scatter

link         chord, sankey
node         adjacency, force_directed
connections  adjacency, force_directed
namePair     adjacency, force_directed
count        adjacency, force_directed
groupId      adjacency, force_directed

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
id           plot
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
