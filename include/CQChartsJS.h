#ifndef CQChartsJS_H
#define CQChartsJS_H

#include <CQChartsPaintDevice.h>
#include <iostream>

namespace CQChartsJS {
  void writeHeadTitle(std::ostream &os, const QString &title) {
    os << "<!doctype html>\n";
    os << "<html lang=\"en\">\n";

    os << "<head>\n";
    os << "<meta charset=\"UTF-8\">\n";
    os << "<title>" << title.toStdString() << "</title>\n";
  }

  void writeToolTipCSS(std::ostream &os) {
    os << "#tooltip {\n";
    os << " font: 28pt sans-serif;\n";
    os << " background: cornsilk;\n";
    os << " border: 1px solid black;\n";
    os << " border-radius: 5px;\n";
    os << " padding: 5px;\n";
    os << "}\n";
    os << "\n";
  }

  void writeCanvasCSS(std::ostream &os) {
    os << "#canvas {\n";
    os << " font: 14pt sans-serif;\n";
    os << " position: fixed;\n";
    os << " left: 0;\n";
    os << " top: 0;\n";
    os << " width: 100%;\n";
    os << " height: 100%;\n";
    os << "}\n";
    os << "\n";
  }

  void writeToolTipProcs(std::ostream &os, const CQChartsPaintDevice::Type &type) {
    if (type == CQChartsPaintDevice::Type::SVG)
      os << "function showTooltip(evt, text) {\n";
    else
      os << "function showTooltip(x, y, text) {\n";

    os << "  let tooltip = document.getElementById(\"tooltip\");\n";
    os << "  tooltip.innerHTML = text;\n";
    os << "  tooltip.style.display = \"block\";\n";

    if (type == CQChartsPaintDevice::Type::SVG) {
      os << "  tooltip.style.left = evt.pageX + 10 + 'px';\n";
      os << "  tooltip.style.top = evt.pageY + 10 + 'px';\n";
    }
    else {
      os << "  tooltip.style.left = x + 10 + 'px';\n";
      os << "  tooltip.style.top = y + 10 + 'px';\n";
    }

    os << "}\n";
    os << "\n";

    os << "function hideTooltip() {\n";
    os << "  var tooltip = document.getElementById(\"tooltip\");\n";
    os << "  tooltip.style.display = \"none\";\n";
    os << "}\n";
    os << "\n";
  }

  void writeLogProc(std::ostream &os) {
    os << "Charts.prototype.log = function(s) {\n";
    os << "  document.getElementById(\"log_message\").innerHTML = s;\n";
    os << "}\n";
    os << "\n";
  }

  void writeWindowFunctions(std::ostream &os) {
    os <<
    "window.addEventListener(\"load\", eventWindowLoaded, false);\n"
    "\n";

    os <<
    "function eventWindowLoaded () {\n"
    "  if (canvasSupport()) {\n"
    "    charts.init();\n"
    "\n"
    "    (function drawFrame () {\n"
    "      var canvas = document.getElementById(\"canvas\");\n"
    "\n"
    "      window.requestAnimationFrame(drawFrame, canvas);\n"
    "\n"
    "      charts.update();\n"
    "    }());\n"
    "  }\n"
    "}\n"
    "\n";

    os <<
    "function canvasSupport() {\n"
    "  return true;\n"
    "}\n"
    "\n";
  }

  void writeEventProcs(std::ostream &os) {
    os <<
    "Charts.prototype.eventMouseDown = function(e) {\n"
    "  charts.plots.forEach(plot => plot.eventMouseDown(e));\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.eventMouseMove = function(e) {\n"
    "  charts.mouseTipObj = null;\n"
    "  charts.plots.forEach(plot => plot.eventMouseMove(e));\n"
    "  if (! charts.mouseTipObj) {\n"
    "    hideTooltip();\n"
    "  }\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.eventMouseUp = function(e) {\n"
    "  charts.plots.forEach(plot => plot.eventMouseUp(e));\n"
    "}\n"
    "\n";
  }

  void writeConvProcs(std::ostream &os) {
    os <<
    "Charts.prototype.plotXToPixel = function(x) {\n"
    "  var sx = (x - this.xmin)/(this.xmax - this.xmin);\n"
    "\n"
    "  var x1 = this.vxmin*this.canvas.width/100.0;\n"
    "  var x2 = this.vxmax*this.canvas.width/100.0;\n"
    "\n"
    "  if (! this.invertX) {\n"
    "    return sx*(x2 - x1) + x1;\n"
    "  }\n"
    "  else {\n"
    "    return sx*(x1 - x2) + x2;\n"
    "  }\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.plotYToPixel = function(y) {\n"
    "  var sy = (y - this.ymin)/(this.ymax - this.ymin);\n"
    "\n"
    "  var y1 = this.canvas.height - this.vymin*this.canvas.height/100.0;\n"
    "  var y2 = this.canvas.height - this.vymax*this.canvas.height/100.0;\n"
    "\n"
    "  if (! this.invertY) {\n"
    "    return sy*(y2 - y1) + y1;\n"
    "  }\n"
    "  else {\n"
    "    return sy*(y1 - y2) + y2;\n"
    "  }\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.plotWidthToPixel = function(w) {\n"
    "  return this.plotXToPixel(w) - this.plotXToPixel(0.0);\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.plotHeightToPixel = function(h) {\n"
    "  return this.plotYToPixel(h) - this.plotYToPixel(0.0);\n"
    "}\n"
    "\n";
  }

  void writeDrawProcs(std::ostream &os) {
    // path procs
    os <<
    "Charts.prototype.moveTo = function(x, y) {\n"
    "  var px = this.plotXToPixel(x);\n"
    "  var py = this.plotYToPixel(y);\n"
    "\n"
    "  this.gc.moveTo(px, py);\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.lineTo = function(x, y) {\n"
    "  var px = this.plotXToPixel(x);\n"
    "  var py = this.plotYToPixel(y);\n"
    "\n"
    "  this.gc.lineTo(px, py);\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.quadTo = function(x1, y1, x2, y2) {\n"
    "  var px1 = this.plotXToPixel(x1);\n"
    "  var py1 = this.plotYToPixel(y1);\n"
    "  var px2 = this.plotXToPixel(x2);\n"
    "  var py2 = this.plotYToPixel(y2);\n"
    "\n"
    "  this.gc.quadraticCurveTo(px, py);\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.curveTo = function(x1, y1, x2, y2, x3, y3) {\n"
    "  var px1 = this.plotXToPixel(x1);\n"
    "  var py1 = this.plotYToPixel(y1);\n"
    "  var px2 = this.plotXToPixel(x2);\n"
    "  var py2 = this.plotYToPixel(y2);\n"
    "  var px3 = this.plotXToPixel(x3);\n"
    "  var py3 = this.plotYToPixel(y3);\n"
    "\n"
    "  this.gc.bezierCurveTo(px1, py1, px2, py2, px3, py3);\n"
    "}\n"
    "\n";

    //---

    os <<
    "Charts.prototype.drawRoundedPolygon = function(x1, y1, x2, y2, xs, ys) {\n"
    "  var px1 = this.plotXToPixel(x1);\n"
    "  var py1 = this.plotYToPixel(y1);\n"
    "  var px2 = this.plotXToPixel(x2);\n"
    "  var py2 = this.plotYToPixel(y2);\n"
    "\n"
    "  this.gc.beginPath();\n"
    "\n"
    "  this.gc.moveTo(px1, py1);\n"
    "  this.gc.lineTo(px2, py1);\n"
    "  this.gc.lineTo(px2, py2);\n"
    "  this.gc.lineTo(px1, py2);\n"
    "\n"
    "  this.gc.closePath();\n"
    "\n"
    "  this.gc.fill();\n"
    "  this.gc.stroke();\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.drawPoint = function(x, y) { \n"
    "  var px = this.plotXToPixel(x);\n"
    "  var py = this.plotYToPixel(y);\n"
    "\n"
    "  this.gc.beginPath();\n"
    "\n"
    "  this.gc.moveTo(px, py);\n"
    "  this.gc.lineTo(px, py);\n"
    "\n"
    "  this.gc.stroke();\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.drawLine = function(x1, y1, x2, y2) { \n"
    "  var px1 = this.plotXToPixel(x1);\n"
    "  var py1 = this.plotYToPixel(y1);\n"
    "  var px2 = this.plotXToPixel(x2);\n"
    "  var py2 = this.plotYToPixel(y2);\n"
    "\n"
    "  this.gc.beginPath();\n"
    "\n"
    "  this.gc.moveTo(px1, py1);\n"
    "  this.gc.lineTo(px2, py2);\n"
    "\n"
    "  this.gc.stroke();\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.strokeRect = function(x1, y1, x2, y2) {\n"
    "  var px1 = this.plotXToPixel(x1);\n"
    "  var py1 = this.plotYToPixel(y1);\n"
    "  var px2 = this.plotXToPixel(x2);\n"
    "  var py2 = this.plotYToPixel(y2);\n"
    "\n"
    "  this.gc.beginPath();\n"
    "\n"
    "  this.gc.moveTo(px1, py1);\n"
    "  this.gc.lineTo(px2, py1);\n"
    "  this.gc.lineTo(px2, py2);\n"
    "  this.gc.lineTo(px1, py2);\n"
    "\n"
    "  this.gc.closePath();\n"
    "\n"
    "  this.gc.stroke();\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.fillRect = function(x1, y1, x2, y2) {\n"
    "  var px1 = this.plotXToPixel(x1);\n"
    "  var py1 = this.plotYToPixel(y1);\n"
    "  var px2 = this.plotXToPixel(x2);\n"
    "  var py2 = this.plotYToPixel(y2);\n"
    "\n"
    "  this.gc.beginPath();\n"
    "\n"
    "  this.gc.moveTo(px1, py1);\n"
    "  this.gc.lineTo(px2, py1);\n"
    "  this.gc.lineTo(px2, py2);\n"
    "  this.gc.lineTo(px1, py2);\n"
    "\n"
    "  this.gc.closePath();\n"
    "\n"
    "  this.gc.fill();\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.drawRect = function(x1, y1, x2, y2) {\n"
    "  var px1 = this.plotXToPixel(x1);\n"
    "  var py1 = this.plotYToPixel(y1);\n"
    "  var px2 = this.plotXToPixel(x2);\n"
    "  var py2 = this.plotYToPixel(y2);\n"
    "\n"
    "  this.gc.beginPath();\n"
    "\n"
    "  this.gc.moveTo(px1, py1);\n"
    "  this.gc.lineTo(px2, py1);\n"
    "  this.gc.lineTo(px2, py2);\n"
    "  this.gc.lineTo(px1, py2);\n"
    "\n"
    "  this.gc.closePath();\n"
    "\n"
    "  this.gc.fill();\n"
    "  this.gc.stroke();\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.drawEllipse = function(x1, y1, x2, y2) {\n"
    "  var px1 = this.plotXToPixel(x1);\n"
    "  var py1 = this.plotYToPixel(y1);\n"
    "  var px2 = this.plotXToPixel(x2);\n"
    "  var py2 = this.plotYToPixel(y2);\n"
    "\n"
    "  var xc = (px1 + px2)/2;\n"
    "  var yc = (py1 + py2)/2;\n"
    "\n"
    "  var w = Math.abs(px2 - px1);\n"
    "  var h = Math.abs(py2 - py1);\n"
    "\n"
    "  var r = (w + h)/4;\n"
    "\n"
    "  var d = r*4*(Math.sqrt(2) - 1)/3;\n"
    "\n"
    "  this.gc.beginPath();\n"
    "\n"
    "  this.gc.moveTo       (                                xc + r, yc    );\n"
    "  this.gc.bezierCurveTo(xc + r, yc + d, xc + d, yc + r, xc    , yc + r);\n"
    "  this.gc.bezierCurveTo(xc - d, yc + r, xc - r, yc + d, xc - r, yc    );\n"
    "  this.gc.bezierCurveTo(xc - r, yc - d, xc - d, yc - r, xc    , yc - r);\n"
    "  this.gc.bezierCurveTo(xc + d, yc - r, xc + r, yc - d, xc + r, yc    );\n"
    "\n"
    "  this.gc.closePath();\n"
    "\n"
    "  this.gc.fill();\n"
    "  this.gc.stroke();\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.drawPolygon = function(poly) {\n"
    "  var np = poly.length;\n"
    "  this.gc.beginPath();\n"
    "  for (var i = 0; i < np; ++i) {\n"
    "    var px = this.plotXToPixel(poly[2*i    ]);\n"
    "    var py = this.plotYToPixel(poly[2*i + 1]);\n"
    "    if (i == 0)\n"
    "      this.gc.moveTo(px, py);\n"
    "    else\n"
    "      this.gc.lineTo(px, py);\n"
    "  }\n"
    "  this.gc.closePath();\n"
    "  this.gc.fill();\n"
    "  this.gc.stroke();\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.drawPolyline = function(poly) {\n"
    "  var np = poly.length;\n"
    "  this.gc.beginPath();\n"
    "  for (var i = 0; i < np; ++i) {\n"
    "    var px = this.plotXToPixel(poly[2*i    ]);\n"
    "    var py = this.plotYToPixel(poly[2*i + 1]);\n"
    "    if (i == 0)\n"
    "      this.gc.moveTo(px, py);\n"
    "    else\n"
    "      this.gc.lineTo(px, py);\n"
    "  }\n"
    "  this.gc.stroke();\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.setFont = function(size) {\n"
    "  this.gc.font = \"\" + (size*window.innerWidth/this.pwidth) + \"px sans-serif\";\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.drawText = function(x, y, text) {\n"
    "  var px = this.plotXToPixel(x);\n"
    "  var py = this.plotYToPixel(y);\n"
    "\n"
    "  var fillStyle = this.gc.fillStyle;\n"
    "  this.gc.fillStyle = this.gc.strokeStyle;\n"
    "  this.gc.fillText(text, px, py);\n"
    "  this.gc.fillStyle = fillStyle;\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.drawRotatedText = function(x, y, text, a) {\n"
    "  var px = this.plotXToPixel(x);\n"
    "  var py = this.plotYToPixel(y);\n"
    "\n"
    "  var c = Math.cos(a);\n"
    "  var s = Math.sin(a);\n"
    "\n"
    "  this.gc.setTransform(c, -s, s, c, px, py);\n"
    "\n"
    "  var fillStyle = this.gc.fillStyle;\n"
    "  this.gc.fillStyle = this.gc.strokeStyle;\n"
    "  this.gc.fillText(text, 0, 0);\n"
    "  this.gc.fillStyle = fillStyle;\n"
    "\n"
    "  this.gc.setTransform(1, 0, 0, 1, 0, 0);\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.setClipRect = function(x1, y1, x2, y2, op) {\n"
    "  var px1 = this.plotXToPixel(x1);\n"
    "  var py1 = this.plotYToPixel(y1);\n"
    "  var px2 = this.plotXToPixel(x2);\n"
    "  var py2 = this.plotYToPixel(y2);\n"
    "\n"
    "  this.gc.beginPath();\n"
    "\n"
    "  this.gc.moveTo(px1, py1);\n"
    "  this.gc.lineTo(px2, py1);\n"
    "  this.gc.lineTo(px2, py2);\n"
    "  this.gc.lineTo(px1, py2);\n"
    "\n"
    "  this.gc.closePath();\n"
    "\n"
    "  this.gc.clip();\n"
    "}\n"
    "\n";
  }

  void writeInsideProcs(std::ostream &os) {
    os <<
    "Charts.prototype.pointInsideRect = function(px, py, xmin, ymin, xmax, ymax) {\n"
    "  var pxmin = this.plotXToPixel(xmin);\n"
    "  var pymax = this.plotYToPixel(ymin);\n"
    "  var pxmax = this.plotXToPixel(xmax);\n"
    "  var pymin = this.plotYToPixel(ymax);\n"
    "  return (px >= Math.min(pxmin, pxmax) && px <= Math.max(pxmin, pxmax) &&\n"
    "          py >= Math.min(pymin, pymax) && py <= Math.max(pymin, pymax));\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.pointInsideCircle = function(px, py, xc, yc, r) {\n"
    "  var pr  = this.plotWidthToPixel(r);\n"
    "  var pxc = this.plotXToPixel(xc);\n"
    "  var pyc = this.plotYToPixel(yc);\n"
    "  var d  = Math.hypot(px - pxc, py - pyc);\n"
    "  return (d <= pr);\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.pointInsidePoly = function(px, py, poly) {\n"
    "  var np = poly.length;\n"
    "  var counter = 0;\n"
    "  var i2 = np - 1;\n"
    "  for (var i1 = 0; i1 < np; ++i1) {\n"
    "    var px1 = this.plotXToPixel(poly[2*i1    ]);\n"
    "    var py1 = this.plotYToPixel(poly[2*i1 + 1]);\n"
    "    var px2 = this.plotXToPixel(poly[2*i2    ]);\n"
    "    var py2 = this.plotYToPixel(poly[2*i2 + 1]);\n"
    "    if (py > Math.min(py1, py2)) {\n"
    "      if (py <= Math.max(py1, py2)) {\n"
    "        if (px <= Math.max(px1, px2)) {\n"
    "          if (py1 != py2) {\n"
    "            var xinters = (py - py1)*(px2 - px1)/(py2 - py1) + px1;\n"
    "            if (px1 == px2 || px <= xinters)\n"
    "              ++counter;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "    }\n"
    "    i2 = i1;\n"
    "  }\n"
    "  return ((counter % 2) != 0);\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.pixelPointPointDistance = function(px1, py1, px2, py2) {\n"
    "  return Math.hypot(px2 - px1, py2 - py1);\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.pixelPointLineDistance = function(px, py, px1, py1, px2, py2) {\n"
    "  var dx1 = px2 - px1;\n"
    "  var dy1 = px2 - py1;\n"
    "\n"
    "  var u2 = dx1*dx1 + dy1*dy1;\n"
    "\n"
    "  if (u2 <= 0.0) { return this.pixelPointPointDistance(px, py, px1, py1); }\n"
    "\n"
    "  var dx2 = px - px1;\n"
    "  var dy2 = py - py1;\n"
    "\n"
    "  var u1 = dx2*dx1 + dy2*dy1;\n"
    "\n"
    "  var u = u1/u2;\n"
    "\n"
    "  if      (u < 0.0) {\n"
    "    return this.pixelPointPointDistance(px, py, px1, py1); }\n"
    "  else if (u > 1.0) {\n"
    "    return this.pixelPointPointDistance(px, py, px2, py2); }\n"
    "  else              {\n"
    "    return this.pixelPointPointDistance(px, py, px1 + u*dx1, py1 + u*dy1); }\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.pointInsidePolyline = function(px, py, poly) {\n"
    "  var np = poly.length;\n"
    "  for (var i2 = 1; i2 < np; ++i2) {\n"
    "    var i1 = i2 - 1;\n"
    "\n"
    "    var px1 = this.plotXToPixel(poly[2*i1  ]);\n"
    "    var py1 = this.plotYToPixel(poly[2*i1+1]);\n"
    "    var px2 = this.plotXToPixel(poly[2*i2  ]);\n"
    "    var py2 = this.plotYToPixel(poly[2*i2+1]);\n"
    "\n"
    "    var r = this.pixelPointLineDistance(px, py, px1, py1, px2, py2);\n"
    "    if (r < 3) return true;\n"
    "  }\n"
    "  return false;\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.normalizeAngle = function(a) {\n"
    "  while (a <    0.0) a += 360.0;\n"
    "  while (a >= 360.0) a -= 360.0;\n"
    "  return a;\n"
    "}\n"
    "\n";

    os <<
    "Charts.prototype.pointInsideArc = function(px, py, arc) {\n"
    "  var cx = this.plotXToPixel(arc.cx);\n"
    "  var cy = this.plotXToPixel(arc.cy);\n"
    "  var r = Math.hypot(px - cx, py - cy);\n"
    "  var ri = this.plotWidthToPixel(arc.ri);\n"
    "  var ro = this.plotWidthToPixel(arc.ro);\n"
    "  if (r < ri || r > ro)\n"
    "    return false;\n"
    "  var a = Math.atan2(cy - py, px - cx)*180.0/Math.PI;\n"
    "  var a = this.normalizeAngle(a);\n"
    "  var a1 = this.normalizeAngle(arc.a1);\n"
    "  var a2 = this.normalizeAngle(arc.a2);\n"
    "  if (a1 > a2) {\n"
    "    if (a >=   0.0 && a <= a2) return true;\n"
    "    if (a <= 360.0 && a >= a1) return true;\n"
    "  }\n"
    "  else {\n"
    "    if (a >= a1 && a <= a2) return true;\n"
    "  }\n"
    "  return false;\n"
    "}\n"
    "\n";
  }
};

#endif
