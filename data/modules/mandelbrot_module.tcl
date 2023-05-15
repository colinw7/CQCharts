set plot1 [create_charts_plot -type empty -xmin -2 -ymin -1.2 -xmax 1.2 -ymax 1.2]
set plot2 [create_charts_plot -type empty -xmin -2 -ymin -1.2 -xmax 1.2 -ymax 1.2]
set plot3 [create_charts_plot -type empty -xmin  0 -ymin 0 -xmax 1 -ymax 1]
set plot4 [create_charts_plot -type empty -xmin  0 -ymin 0 -xmax 1 -ymax 1]

set_charts_property -plot $plot1 -name module.name -value mandelbrot
set_charts_property -plot $plot2 -name module.name -value {mandelbrot overview}
set_charts_property -plot $plot3 -name module.name -value {mandelbrot probe_x}
set_charts_property -plot $plot4 -name module.name -value {mandelbrot probe_y}

set_charts_property -plot $plot3 -name xaxis.visible -value 1
set_charts_property -plot $plot3 -name yaxis.visible -value 1
set_charts_property -plot $plot4 -name xaxis.visible -value 1
set_charts_property -plot $plot4 -name yaxis.visible -value 1

set view [get_charts_data -plot $plot1 -name view]

set_charts_property -view $view -name probe.objects -value 0

set_charts_property -plot $plot1 -name range.view -value {0 0 50 50}
set_charts_property -plot $plot2 -name range.view -value {50 50 100 100}
set_charts_property -plot $plot3 -name range.view -value {50 0 100 50}
set_charts_property -plot $plot4 -name range.view -value {0 50 50 100}
