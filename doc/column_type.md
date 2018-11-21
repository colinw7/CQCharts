# Column Types #

The following column types are supported
 + string
 + boolean
 + real
 + integer
 + time
 + rect
 + polygon
 + polygon list
 + connections list
 + name pair
 + path
 + style
 + color
 + image
 + symbol type
 + symbol size
 + font size

Values can be converted in the model (edit role) or in the plot (display role).

For example time values are usually stored as date strings in the input data so
to store the actual UNIX time value in the edit role of the model they need to
be converted using the column 'iformat' name value. To display the time in the
plot (e.g. on axis tick labels) this converted time needs to be converted back
to a string using the 'oformat' name value.

Color values can either be stored as color names in the input data or can
be mapped into a palette from numeric values or discrete strings. Usually
mapping takes place on data that can be used for other parts of the plot
so it is better to convert the value in the plot rather than the model so
the original data can still be accessed.
