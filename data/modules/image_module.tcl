set plot [create_charts_plot -type empty -xmin 0 -ymin 0 -xmax 1 -ymax 1]

#set_charts_property -plot $plot -name module.name -value {image pics/cathedral.png}
set_charts_property -plot $plot -name module.name -value {image pics/Catwoman.jpg}

set view [get_charts_data -plot $plot -name view]

set_charts_property -view $view -name probe.objects -value 0

set_charts_property -plot $plot -name range.view -value {0 0 100 100}
