#include <CQChartsTheme.h>
#include <CQChartsGradientPalette.h>
#include <cassert>

class CQChartsDefaultPalette : public CQChartsGradientPalette {
 public:
  CQChartsDefaultPalette() {
    setDesc("Default");
  }
};

class CQChartsDefinedPalette : public CQChartsGradientPalette {
 public:
  CQChartsDefinedPalette() {
    setColorType(CQChartsGradientPalette::ColorType::DEFINED);

    resetDefinedColors();
  }

  void addRGBColor(double v, double r, double g, double b) {
    addDefinedColor(v, QColor::fromRgbF(r, g, b));
  }
};

class CQChartsPalette1 : public CQChartsDefinedPalette {
 public:
  CQChartsPalette1() {
    setDesc("Palette 1");

    setRedModel  (1);
    setGreenModel(7);
    setBlueModel (4);

    setBlueNegative(true);

    setRedMax  (0.8);
    setGreenMax(0.4);

    addDefinedColor(0.0, QColor("#6d78ad"));
    addDefinedColor(1.0, QColor("#51cda0"));
    addDefinedColor(2.0, QColor("#df7970"));
  }
};

class CQChartsPalette2 : public CQChartsDefinedPalette {
 public:
  CQChartsPalette2() {
    setDesc("Palette 2");

    setRedModel  (1);
    setGreenModel(7);
    setBlueModel (4);

    setBlueNegative(true);

    setRedMax  (0.8);
    setGreenMax(0.4);

    addDefinedColor(0.0, QColor("#4d81bc"));
    addDefinedColor(1.0, QColor("#c0504e"));
    addDefinedColor(2.0, QColor("#9bbb58"));
  }
};

//---

class CQChartsPaletteAccent : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteAccent() {
    addDefinedColor(0.0, "#7FC97F");
    addDefinedColor(1.0, "#BEAED4");
    addDefinedColor(2.0, "#FDC086");
    addDefinedColor(3.0, "#FFFF99");
    addDefinedColor(4.0, "#386CB0");
    addDefinedColor(5.0, "#F0027F");
    addDefinedColor(6.0, "#BF5B17");
    addDefinedColor(7.0, "#666666");
  }
};

class CQChartsPaletteBlues : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteBlues() {
    addDefinedColor(0.0, QColor("#F7FBFF"));
    addDefinedColor(1.0, QColor("#DEEBF7"));
    addDefinedColor(2.0, QColor("#C6DBEF"));
    addDefinedColor(3.0, QColor("#9ECAE1"));
    addDefinedColor(4.0, QColor("#6BAED6"));
    addDefinedColor(5.0, QColor("#4292C6"));
    addDefinedColor(6.0, QColor("#2171B5"));
    addDefinedColor(7.0, QColor("#084594"));
  }
};

class CQChartsPaletteBrbg : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteBrbg() {
    addDefinedColor(0.0, QColor("#8C510A"));
    addDefinedColor(1.0, QColor("#BF812D"));
    addDefinedColor(2.0, QColor("#DFC27D"));
    addDefinedColor(3.0, QColor("#F6E8C3"));
    addDefinedColor(4.0, QColor("#C7EAE5"));
    addDefinedColor(5.0, QColor("#80CDC1"));
    addDefinedColor(6.0, QColor("#35978F"));
    addDefinedColor(7.0, QColor("#01665E"));
  }
};

class CQChartsPaletteBugn : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteBugn() {
    addDefinedColor(0.0, QColor("#F7FCFD"));
    addDefinedColor(1.0, QColor("#E5F5F9"));
    addDefinedColor(2.0, QColor("#CCECE6"));
    addDefinedColor(3.0, QColor("#99D8C9"));
    addDefinedColor(4.0, QColor("#66C2A4"));
    addDefinedColor(5.0, QColor("#41AE76"));
    addDefinedColor(6.0, QColor("#238B45"));
    addDefinedColor(7.0, QColor("#005824"));
  }
};

class CQChartsPaletteBupu : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteBupu() {
    addDefinedColor(0.0, QColor("#F7FCFD"));
    addDefinedColor(1.0, QColor("#E0ECF4"));
    addDefinedColor(2.0, QColor("#BFD3E6"));
    addDefinedColor(3.0, QColor("#9EBCDA"));
    addDefinedColor(4.0, QColor("#8C96C6"));
    addDefinedColor(5.0, QColor("#8C6BB1"));
    addDefinedColor(6.0, QColor("#88419D"));
    addDefinedColor(7.0, QColor("#6E016B"));
  }
};

class CQChartsPaletteChromajs : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteChromajs() {
    addDefinedColor(0.0, QColor("#ffffe0"));
    addDefinedColor(1.0, QColor("#ffdfb8"));
    addDefinedColor(2.0, QColor("#ffbc94"));
    addDefinedColor(3.0, QColor("#ff9777"));
    addDefinedColor(4.0, QColor("#ff6962"));
    addDefinedColor(5.0, QColor("#ee4256"));
    addDefinedColor(6.0, QColor("#d21f47"));
    addDefinedColor(7.0, QColor("#b0062c"));
    addDefinedColor(8.0, QColor("#8b0000"));
  }
};

class CQChartsPaletteDark2 : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteDark2() {
    addDefinedColor(0.0, QColor("#1B9E77"));
    addDefinedColor(1.0, QColor("#D95F02"));
    addDefinedColor(2.0, QColor("#7570B3"));
    addDefinedColor(3.0, QColor("#E7298A"));
    addDefinedColor(4.0, QColor("#66A61E"));
    addDefinedColor(5.0, QColor("#E6AB02"));
    addDefinedColor(6.0, QColor("#A6761D"));
    addDefinedColor(7.0, QColor("#666666"));
  }
};

class CQChartsPaletteGnbu : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteGnbu() {
    addDefinedColor(0.0, QColor("#F7FCF0"));
    addDefinedColor(1.0, QColor("#E0F3DB"));
    addDefinedColor(2.0, QColor("#CCEBC5"));
    addDefinedColor(3.0, QColor("#A8DDB5"));
    addDefinedColor(4.0, QColor("#7BCCC4"));
    addDefinedColor(5.0, QColor("#4EB3D3"));
    addDefinedColor(6.0, QColor("#2B8CBE"));
    addDefinedColor(7.0, QColor("#08589E"));
  }
};

class CQChartsPaletteGnpu : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteGnpu() {
    addDefinedColor( 1.0, QColor("#396353"));
    addDefinedColor( 2.0, QColor("#0db14b"));
    addDefinedColor( 3.0, QColor("#6dc067"));
    addDefinedColor( 4.0, QColor("#abd69b"));
    addDefinedColor( 5.0, QColor("#daeac1"));
    addDefinedColor( 6.0, QColor("#dfcce4"));
    addDefinedColor( 7.0, QColor("#c7b2d6"));
    addDefinedColor( 8.0, QColor("#9474b4"));
    addDefinedColor( 9.0, QColor("#754098"));
    addDefinedColor(10.0, QColor("#504971"));
  }
};

class CQChartsPaletteGreens : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteGreens() {
    addDefinedColor(0.0, QColor("#F7FCF5"));
    addDefinedColor(1.0, QColor("#E5F5E0"));
    addDefinedColor(2.0, QColor("#C7E9C0"));
    addDefinedColor(3.0, QColor("#A1D99B"));
    addDefinedColor(4.0, QColor("#74C476"));
    addDefinedColor(5.0, QColor("#41AB5D"));
    addDefinedColor(6.0, QColor("#238B45"));
    addDefinedColor(7.0, QColor("#005A32"));
  }
};

class CQChartsPaletteGreys : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteGreys() {
    addDefinedColor(0.0, QColor("#FFFFFF"));
    addDefinedColor(1.0, QColor("#F0F0F0"));
    addDefinedColor(2.0, QColor("#D9D9D9"));
    addDefinedColor(3.0, QColor("#BDBDBD"));
    addDefinedColor(4.0, QColor("#969696"));
    addDefinedColor(5.0, QColor("#737373"));
    addDefinedColor(6.0, QColor("#525252"));
    addDefinedColor(7.0, QColor("#252525"));
  }
};

class CQChartsPaletteInferno : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteInferno() {
    int n = 0;

    addRGBColor(n++, 0.001462, 0.000466, 0.013866);
    addRGBColor(n++, 0.002267, 0.001270, 0.018570);
    addRGBColor(n++, 0.003299, 0.002249, 0.024239);
    addRGBColor(n++, 0.004547, 0.003392, 0.030909);
    addRGBColor(n++, 0.006006, 0.004692, 0.038558);
    addRGBColor(n++, 0.007676, 0.006136, 0.046836);
    addRGBColor(n++, 0.009561, 0.007713, 0.055143);
    addRGBColor(n++, 0.011663, 0.009417, 0.063460);
    addRGBColor(n++, 0.013995, 0.011225, 0.071862);
    addRGBColor(n++, 0.016561, 0.013136, 0.080282);
    addRGBColor(n++, 0.019373, 0.015133, 0.088767);
    addRGBColor(n++, 0.022447, 0.017199, 0.097327);
    addRGBColor(n++, 0.025793, 0.019331, 0.105930);
    addRGBColor(n++, 0.029432, 0.021503, 0.114621);
    addRGBColor(n++, 0.033385, 0.023702, 0.123397);
    addRGBColor(n++, 0.037668, 0.025921, 0.132232);
    addRGBColor(n++, 0.042253, 0.028139, 0.141141);
    addRGBColor(n++, 0.046915, 0.030324, 0.150164);
    addRGBColor(n++, 0.051644, 0.032474, 0.159254);
    addRGBColor(n++, 0.056449, 0.034569, 0.168414);
    addRGBColor(n++, 0.061340, 0.036590, 0.177642);
    addRGBColor(n++, 0.066331, 0.038504, 0.186962);
    addRGBColor(n++, 0.071429, 0.040294, 0.196354);
    addRGBColor(n++, 0.076637, 0.041905, 0.205799);
    addRGBColor(n++, 0.081962, 0.043328, 0.215289);
    addRGBColor(n++, 0.087411, 0.044556, 0.224813);
    addRGBColor(n++, 0.092990, 0.045583, 0.234358);
    addRGBColor(n++, 0.098702, 0.046402, 0.243904);
    addRGBColor(n++, 0.104551, 0.047008, 0.253430);
    addRGBColor(n++, 0.110536, 0.047399, 0.262912);
    addRGBColor(n++, 0.116656, 0.047574, 0.272321);
    addRGBColor(n++, 0.122908, 0.047536, 0.281624);
    addRGBColor(n++, 0.129285, 0.047293, 0.290788);
    addRGBColor(n++, 0.135778, 0.046856, 0.299776);
    addRGBColor(n++, 0.142378, 0.046242, 0.308553);
    addRGBColor(n++, 0.149073, 0.045468, 0.317085);
    addRGBColor(n++, 0.155850, 0.044559, 0.325338);
    addRGBColor(n++, 0.162689, 0.043554, 0.333277);
    addRGBColor(n++, 0.169575, 0.042489, 0.340874);
    addRGBColor(n++, 0.176493, 0.041402, 0.348111);
    addRGBColor(n++, 0.183429, 0.040329, 0.354971);
    addRGBColor(n++, 0.190367, 0.039309, 0.361447);
    addRGBColor(n++, 0.197297, 0.038400, 0.367535);
    addRGBColor(n++, 0.204209, 0.037632, 0.373238);
    addRGBColor(n++, 0.211095, 0.037030, 0.378563);
    addRGBColor(n++, 0.217949, 0.036615, 0.383522);
    addRGBColor(n++, 0.224763, 0.036405, 0.388129);
    addRGBColor(n++, 0.231538, 0.036405, 0.392400);
    addRGBColor(n++, 0.238273, 0.036621, 0.396353);
    addRGBColor(n++, 0.244967, 0.037055, 0.400007);
    addRGBColor(n++, 0.251620, 0.037705, 0.403378);
    addRGBColor(n++, 0.258234, 0.038571, 0.406485);
    addRGBColor(n++, 0.264810, 0.039647, 0.409345);
    addRGBColor(n++, 0.271347, 0.040922, 0.411976);
    addRGBColor(n++, 0.277850, 0.042353, 0.414392);
    addRGBColor(n++, 0.284321, 0.043933, 0.416608);
    addRGBColor(n++, 0.290763, 0.045644, 0.418637);
    addRGBColor(n++, 0.297178, 0.047470, 0.420491);
    addRGBColor(n++, 0.303568, 0.049396, 0.422182);
    addRGBColor(n++, 0.309935, 0.051407, 0.423721);
    addRGBColor(n++, 0.316282, 0.053490, 0.425116);
    addRGBColor(n++, 0.322610, 0.055634, 0.426377);
    addRGBColor(n++, 0.328921, 0.057827, 0.427511);
    addRGBColor(n++, 0.335217, 0.060060, 0.428524);
    addRGBColor(n++, 0.341500, 0.062325, 0.429425);
    addRGBColor(n++, 0.347771, 0.064616, 0.430217);
    addRGBColor(n++, 0.354032, 0.066925, 0.430906);
    addRGBColor(n++, 0.360284, 0.069247, 0.431497);
    addRGBColor(n++, 0.366529, 0.071579, 0.431994);
    addRGBColor(n++, 0.372768, 0.073915, 0.432400);
    addRGBColor(n++, 0.379001, 0.076253, 0.432719);
    addRGBColor(n++, 0.385228, 0.078591, 0.432955);
    addRGBColor(n++, 0.391453, 0.080927, 0.433109);
    addRGBColor(n++, 0.397674, 0.083257, 0.433183);
    addRGBColor(n++, 0.403894, 0.085580, 0.433179);
    addRGBColor(n++, 0.410113, 0.087896, 0.433098);
    addRGBColor(n++, 0.416331, 0.090203, 0.432943);
    addRGBColor(n++, 0.422549, 0.092501, 0.432714);
    addRGBColor(n++, 0.428768, 0.094790, 0.432412);
    addRGBColor(n++, 0.434987, 0.097069, 0.432039);
    addRGBColor(n++, 0.441207, 0.099338, 0.431594);
    addRGBColor(n++, 0.447428, 0.101597, 0.431080);
    addRGBColor(n++, 0.453651, 0.103848, 0.430498);
    addRGBColor(n++, 0.459875, 0.106089, 0.429846);
    addRGBColor(n++, 0.466100, 0.108322, 0.429125);
    addRGBColor(n++, 0.472328, 0.110547, 0.428334);
    addRGBColor(n++, 0.478558, 0.112764, 0.427475);
    addRGBColor(n++, 0.484789, 0.114974, 0.426548);
    addRGBColor(n++, 0.491022, 0.117179, 0.425552);
    addRGBColor(n++, 0.497257, 0.119379, 0.424488);
    addRGBColor(n++, 0.503493, 0.121575, 0.423356);
    addRGBColor(n++, 0.509730, 0.123769, 0.422156);
    addRGBColor(n++, 0.515967, 0.125960, 0.420887);
    addRGBColor(n++, 0.522206, 0.128150, 0.419549);
    addRGBColor(n++, 0.528444, 0.130341, 0.418142);
    addRGBColor(n++, 0.534683, 0.132534, 0.416667);
    addRGBColor(n++, 0.540920, 0.134729, 0.415123);
    addRGBColor(n++, 0.547157, 0.136929, 0.413511);
    addRGBColor(n++, 0.553392, 0.139134, 0.411829);
    addRGBColor(n++, 0.559624, 0.141346, 0.410078);
    addRGBColor(n++, 0.565854, 0.143567, 0.408258);
    addRGBColor(n++, 0.572081, 0.145797, 0.406369);
    addRGBColor(n++, 0.578304, 0.148039, 0.404411);
    addRGBColor(n++, 0.584521, 0.150294, 0.402385);
    addRGBColor(n++, 0.590734, 0.152563, 0.400290);
    addRGBColor(n++, 0.596940, 0.154848, 0.398125);
    addRGBColor(n++, 0.603139, 0.157151, 0.395891);
    addRGBColor(n++, 0.609330, 0.159474, 0.393589);
    addRGBColor(n++, 0.615513, 0.161817, 0.391219);
    addRGBColor(n++, 0.621685, 0.164184, 0.388781);
    addRGBColor(n++, 0.627847, 0.166575, 0.386276);
    addRGBColor(n++, 0.633998, 0.168992, 0.383704);
    addRGBColor(n++, 0.640135, 0.171438, 0.381065);
    addRGBColor(n++, 0.646260, 0.173914, 0.378359);
    addRGBColor(n++, 0.652369, 0.176421, 0.375586);
    addRGBColor(n++, 0.658463, 0.178962, 0.372748);
    addRGBColor(n++, 0.664540, 0.181539, 0.369846);
    addRGBColor(n++, 0.670599, 0.184153, 0.366879);
    addRGBColor(n++, 0.676638, 0.186807, 0.363849);
    addRGBColor(n++, 0.682656, 0.189501, 0.360757);
    addRGBColor(n++, 0.688653, 0.192239, 0.357603);
    addRGBColor(n++, 0.694627, 0.195021, 0.354388);
    addRGBColor(n++, 0.700576, 0.197851, 0.351113);
    addRGBColor(n++, 0.706500, 0.200728, 0.347777);
    addRGBColor(n++, 0.712396, 0.203656, 0.344383);
    addRGBColor(n++, 0.718264, 0.206636, 0.340931);
    addRGBColor(n++, 0.724103, 0.209670, 0.337424);
    addRGBColor(n++, 0.729909, 0.212759, 0.333861);
    addRGBColor(n++, 0.735683, 0.215906, 0.330245);
    addRGBColor(n++, 0.741423, 0.219112, 0.326576);
    addRGBColor(n++, 0.747127, 0.222378, 0.322856);
    addRGBColor(n++, 0.752794, 0.225706, 0.319085);
    addRGBColor(n++, 0.758422, 0.229097, 0.315266);
    addRGBColor(n++, 0.764010, 0.232554, 0.311399);
    addRGBColor(n++, 0.769556, 0.236077, 0.307485);
    addRGBColor(n++, 0.775059, 0.239667, 0.303526);
    addRGBColor(n++, 0.780517, 0.243327, 0.299523);
    addRGBColor(n++, 0.785929, 0.247056, 0.295477);
    addRGBColor(n++, 0.791293, 0.250856, 0.291390);
    addRGBColor(n++, 0.796607, 0.254728, 0.287264);
    addRGBColor(n++, 0.801871, 0.258674, 0.283099);
    addRGBColor(n++, 0.807082, 0.262692, 0.278898);
    addRGBColor(n++, 0.812239, 0.266786, 0.274661);
    addRGBColor(n++, 0.817341, 0.270954, 0.270390);
    addRGBColor(n++, 0.822386, 0.275197, 0.266085);
    addRGBColor(n++, 0.827372, 0.279517, 0.261750);
    addRGBColor(n++, 0.832299, 0.283913, 0.257383);
    addRGBColor(n++, 0.837165, 0.288385, 0.252988);
    addRGBColor(n++, 0.841969, 0.292933, 0.248564);
    addRGBColor(n++, 0.846709, 0.297559, 0.244113);
    addRGBColor(n++, 0.851384, 0.302260, 0.239636);
    addRGBColor(n++, 0.855992, 0.307038, 0.235133);
    addRGBColor(n++, 0.860533, 0.311892, 0.230606);
    addRGBColor(n++, 0.865006, 0.316822, 0.226055);
    addRGBColor(n++, 0.869409, 0.321827, 0.221482);
    addRGBColor(n++, 0.873741, 0.326906, 0.216886);
    addRGBColor(n++, 0.878001, 0.332060, 0.212268);
    addRGBColor(n++, 0.882188, 0.337287, 0.207628);
    addRGBColor(n++, 0.886302, 0.342586, 0.202968);
    addRGBColor(n++, 0.890341, 0.347957, 0.198286);
    addRGBColor(n++, 0.894305, 0.353399, 0.193584);
    addRGBColor(n++, 0.898192, 0.358911, 0.188860);
    addRGBColor(n++, 0.902003, 0.364492, 0.184116);
    addRGBColor(n++, 0.905735, 0.370140, 0.179350);
    addRGBColor(n++, 0.909390, 0.375856, 0.174563);
    addRGBColor(n++, 0.912966, 0.381636, 0.169755);
    addRGBColor(n++, 0.916462, 0.387481, 0.164924);
    addRGBColor(n++, 0.919879, 0.393389, 0.160070);
    addRGBColor(n++, 0.923215, 0.399359, 0.155193);
    addRGBColor(n++, 0.926470, 0.405389, 0.150292);
    addRGBColor(n++, 0.929644, 0.411479, 0.145367);
    addRGBColor(n++, 0.932737, 0.417627, 0.140417);
    addRGBColor(n++, 0.935747, 0.423831, 0.135440);
    addRGBColor(n++, 0.938675, 0.430091, 0.130438);
    addRGBColor(n++, 0.941521, 0.436405, 0.125409);
    addRGBColor(n++, 0.944285, 0.442772, 0.120354);
    addRGBColor(n++, 0.946965, 0.449191, 0.115272);
    addRGBColor(n++, 0.949562, 0.455660, 0.110164);
    addRGBColor(n++, 0.952075, 0.462178, 0.105031);
    addRGBColor(n++, 0.954506, 0.468744, 0.099874);
    addRGBColor(n++, 0.956852, 0.475356, 0.094695);
    addRGBColor(n++, 0.959114, 0.482014, 0.089499);
    addRGBColor(n++, 0.961293, 0.488716, 0.084289);
    addRGBColor(n++, 0.963387, 0.495462, 0.079073);
    addRGBColor(n++, 0.965397, 0.502249, 0.073859);
    addRGBColor(n++, 0.967322, 0.509078, 0.068659);
    addRGBColor(n++, 0.969163, 0.515946, 0.063488);
    addRGBColor(n++, 0.970919, 0.522853, 0.058367);
    addRGBColor(n++, 0.972590, 0.529798, 0.053324);
    addRGBColor(n++, 0.974176, 0.536780, 0.048392);
    addRGBColor(n++, 0.975677, 0.543798, 0.043618);
    addRGBColor(n++, 0.977092, 0.550850, 0.039050);
    addRGBColor(n++, 0.978422, 0.557937, 0.034931);
    addRGBColor(n++, 0.979666, 0.565057, 0.031409);
    addRGBColor(n++, 0.980824, 0.572209, 0.028508);
    addRGBColor(n++, 0.981895, 0.579392, 0.026250);
    addRGBColor(n++, 0.982881, 0.586606, 0.024661);
    addRGBColor(n++, 0.983779, 0.593849, 0.023770);
    addRGBColor(n++, 0.984591, 0.601122, 0.023606);
    addRGBColor(n++, 0.985315, 0.608422, 0.024202);
    addRGBColor(n++, 0.985952, 0.615750, 0.025592);
    addRGBColor(n++, 0.986502, 0.623105, 0.027814);
    addRGBColor(n++, 0.986964, 0.630485, 0.030908);
    addRGBColor(n++, 0.987337, 0.637890, 0.034916);
    addRGBColor(n++, 0.987622, 0.645320, 0.039886);
    addRGBColor(n++, 0.987819, 0.652773, 0.045581);
    addRGBColor(n++, 0.987926, 0.660250, 0.051750);
    addRGBColor(n++, 0.987945, 0.667748, 0.058329);
    addRGBColor(n++, 0.987874, 0.675267, 0.065257);
    addRGBColor(n++, 0.987714, 0.682807, 0.072489);
    addRGBColor(n++, 0.987464, 0.690366, 0.079990);
    addRGBColor(n++, 0.987124, 0.697944, 0.087731);
    addRGBColor(n++, 0.986694, 0.705540, 0.095694);
    addRGBColor(n++, 0.986175, 0.713153, 0.103863);
    addRGBColor(n++, 0.985566, 0.720782, 0.112229);
    addRGBColor(n++, 0.984865, 0.728427, 0.120785);
    addRGBColor(n++, 0.984075, 0.736087, 0.129527);
    addRGBColor(n++, 0.983196, 0.743758, 0.138453);
    addRGBColor(n++, 0.982228, 0.751442, 0.147565);
    addRGBColor(n++, 0.981173, 0.759135, 0.156863);
    addRGBColor(n++, 0.980032, 0.766837, 0.166353);
    addRGBColor(n++, 0.978806, 0.774545, 0.176037);
    addRGBColor(n++, 0.977497, 0.782258, 0.185923);
    addRGBColor(n++, 0.976108, 0.789974, 0.196018);
    addRGBColor(n++, 0.974638, 0.797692, 0.206332);
    addRGBColor(n++, 0.973088, 0.805409, 0.216877);
    addRGBColor(n++, 0.971468, 0.813122, 0.227658);
    addRGBColor(n++, 0.969783, 0.820825, 0.238686);
    addRGBColor(n++, 0.968041, 0.828515, 0.249972);
    addRGBColor(n++, 0.966243, 0.836191, 0.261534);
    addRGBColor(n++, 0.964394, 0.843848, 0.273391);
    addRGBColor(n++, 0.962517, 0.851476, 0.285546);
    addRGBColor(n++, 0.960626, 0.859069, 0.298010);
    addRGBColor(n++, 0.958720, 0.866624, 0.310820);
    addRGBColor(n++, 0.956834, 0.874129, 0.323974);
    addRGBColor(n++, 0.954997, 0.881569, 0.337475);
    addRGBColor(n++, 0.953215, 0.888942, 0.351369);
    addRGBColor(n++, 0.951546, 0.896226, 0.365627);
    addRGBColor(n++, 0.950018, 0.903409, 0.380271);
    addRGBColor(n++, 0.948683, 0.910473, 0.395289);
    addRGBColor(n++, 0.947594, 0.917399, 0.410665);
    addRGBColor(n++, 0.946809, 0.924168, 0.426373);
    addRGBColor(n++, 0.946392, 0.930761, 0.442367);
    addRGBColor(n++, 0.946403, 0.937159, 0.458592);
    addRGBColor(n++, 0.946903, 0.943348, 0.474970);
    addRGBColor(n++, 0.947937, 0.949318, 0.491426);
    addRGBColor(n++, 0.949545, 0.955063, 0.507860);
    addRGBColor(n++, 0.951740, 0.960587, 0.524203);
    addRGBColor(n++, 0.954529, 0.965896, 0.540361);
    addRGBColor(n++, 0.957896, 0.971003, 0.556275);
    addRGBColor(n++, 0.961812, 0.975924, 0.571925);
    addRGBColor(n++, 0.966249, 0.980678, 0.587206);
    addRGBColor(n++, 0.971162, 0.985282, 0.602154);
    addRGBColor(n++, 0.976511, 0.989753, 0.616760);
    addRGBColor(n++, 0.982257, 0.994109, 0.631017);
    addRGBColor(n++, 0.988362, 0.998364, 0.644924);
  }
};

class CQChartsPaletteJet : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteJet() {
    addRGBColor(0, 0.0, 0.0, 0.5);
    addRGBColor(1, 0.0, 0.0, 1.0);
    addRGBColor(2, 0.0, 0.5, 1.0);
    addRGBColor(3, 0.0, 1.0, 1.0);
    addRGBColor(4, 0.5, 1.0, 0.5);
    addRGBColor(5, 1.0, 1.0, 0.0);
    addRGBColor(6, 1.0, 0.5, 0.0);
    addRGBColor(7, 1.0, 0.0, 0.0);
    addRGBColor(8, 0.5, 0.0, 0.0);
  }
};

class CQChartsPaletteMagma : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteMagma() {
    int n = 0;

    addRGBColor(n++, 0.001462, 0.000466, 0.013866);
    addRGBColor(n++, 0.002258, 0.001295, 0.018331);
    addRGBColor(n++, 0.003279, 0.002305, 0.023708);
    addRGBColor(n++, 0.004512, 0.003490, 0.029965);
    addRGBColor(n++, 0.005950, 0.004843, 0.037130);
    addRGBColor(n++, 0.007588, 0.006356, 0.044973);
    addRGBColor(n++, 0.009426, 0.008022, 0.052844);
    addRGBColor(n++, 0.011465, 0.009828, 0.060750);
    addRGBColor(n++, 0.013708, 0.011771, 0.068667);
    addRGBColor(n++, 0.016156, 0.013840, 0.076603);
    addRGBColor(n++, 0.018815, 0.016026, 0.084584);
    addRGBColor(n++, 0.021692, 0.018320, 0.092610);
    addRGBColor(n++, 0.024792, 0.020715, 0.100676);
    addRGBColor(n++, 0.028123, 0.023201, 0.108787);
    addRGBColor(n++, 0.031696, 0.025765, 0.116965);
    addRGBColor(n++, 0.035520, 0.028397, 0.125209);
    addRGBColor(n++, 0.039608, 0.031090, 0.133515);
    addRGBColor(n++, 0.043830, 0.033830, 0.141886);
    addRGBColor(n++, 0.048062, 0.036607, 0.150327);
    addRGBColor(n++, 0.052320, 0.039407, 0.158841);
    addRGBColor(n++, 0.056615, 0.042160, 0.167446);
    addRGBColor(n++, 0.060949, 0.044794, 0.176129);
    addRGBColor(n++, 0.065330, 0.047318, 0.184892);
    addRGBColor(n++, 0.069764, 0.049726, 0.193735);
    addRGBColor(n++, 0.074257, 0.052017, 0.202660);
    addRGBColor(n++, 0.078815, 0.054184, 0.211667);
    addRGBColor(n++, 0.083446, 0.056225, 0.220755);
    addRGBColor(n++, 0.088155, 0.058133, 0.229922);
    addRGBColor(n++, 0.092949, 0.059904, 0.239164);
    addRGBColor(n++, 0.097833, 0.061531, 0.248477);
    addRGBColor(n++, 0.102815, 0.063010, 0.257854);
    addRGBColor(n++, 0.107899, 0.064335, 0.267289);
    addRGBColor(n++, 0.113094, 0.065492, 0.276784);
    addRGBColor(n++, 0.118405, 0.066479, 0.286321);
    addRGBColor(n++, 0.123833, 0.067295, 0.295879);
    addRGBColor(n++, 0.129380, 0.067935, 0.305443);
    addRGBColor(n++, 0.135053, 0.068391, 0.315000);
    addRGBColor(n++, 0.140858, 0.068654, 0.324538);
    addRGBColor(n++, 0.146785, 0.068738, 0.334011);
    addRGBColor(n++, 0.152839, 0.068637, 0.343404);
    addRGBColor(n++, 0.159018, 0.068354, 0.352688);
    addRGBColor(n++, 0.165308, 0.067911, 0.361816);
    addRGBColor(n++, 0.171713, 0.067305, 0.370771);
    addRGBColor(n++, 0.178212, 0.066576, 0.379497);
    addRGBColor(n++, 0.184801, 0.065732, 0.387973);
    addRGBColor(n++, 0.191460, 0.064818, 0.396152);
    addRGBColor(n++, 0.198177, 0.063862, 0.404009);
    addRGBColor(n++, 0.204935, 0.062907, 0.411514);
    addRGBColor(n++, 0.211718, 0.061992, 0.418647);
    addRGBColor(n++, 0.218512, 0.061158, 0.425392);
    addRGBColor(n++, 0.225302, 0.060445, 0.431742);
    addRGBColor(n++, 0.232077, 0.059889, 0.437695);
    addRGBColor(n++, 0.238826, 0.059517, 0.443256);
    addRGBColor(n++, 0.245543, 0.059352, 0.448436);
    addRGBColor(n++, 0.252220, 0.059415, 0.453248);
    addRGBColor(n++, 0.258857, 0.059706, 0.457710);
    addRGBColor(n++, 0.265447, 0.060237, 0.461840);
    addRGBColor(n++, 0.271994, 0.060994, 0.465660);
    addRGBColor(n++, 0.278493, 0.061978, 0.469190);
    addRGBColor(n++, 0.284951, 0.063168, 0.472451);
    addRGBColor(n++, 0.291366, 0.064553, 0.475462);
    addRGBColor(n++, 0.297740, 0.066117, 0.478243);
    addRGBColor(n++, 0.304081, 0.067835, 0.480812);
    addRGBColor(n++, 0.310382, 0.069702, 0.483186);
    addRGBColor(n++, 0.316654, 0.071690, 0.485380);
    addRGBColor(n++, 0.322899, 0.073782, 0.487408);
    addRGBColor(n++, 0.329114, 0.075972, 0.489287);
    addRGBColor(n++, 0.335308, 0.078236, 0.491024);
    addRGBColor(n++, 0.341482, 0.080564, 0.492631);
    addRGBColor(n++, 0.347636, 0.082946, 0.494121);
    addRGBColor(n++, 0.353773, 0.085373, 0.495501);
    addRGBColor(n++, 0.359898, 0.087831, 0.496778);
    addRGBColor(n++, 0.366012, 0.090314, 0.497960);
    addRGBColor(n++, 0.372116, 0.092816, 0.499053);
    addRGBColor(n++, 0.378211, 0.095332, 0.500067);
    addRGBColor(n++, 0.384299, 0.097855, 0.501002);
    addRGBColor(n++, 0.390384, 0.100379, 0.501864);
    addRGBColor(n++, 0.396467, 0.102902, 0.502658);
    addRGBColor(n++, 0.402548, 0.105420, 0.503386);
    addRGBColor(n++, 0.408629, 0.107930, 0.504052);
    addRGBColor(n++, 0.414709, 0.110431, 0.504662);
    addRGBColor(n++, 0.420791, 0.112920, 0.505215);
    addRGBColor(n++, 0.426877, 0.115395, 0.505714);
    addRGBColor(n++, 0.432967, 0.117855, 0.506160);
    addRGBColor(n++, 0.439062, 0.120298, 0.506555);
    addRGBColor(n++, 0.445163, 0.122724, 0.506901);
    addRGBColor(n++, 0.451271, 0.125132, 0.507198);
    addRGBColor(n++, 0.457386, 0.127522, 0.507448);
    addRGBColor(n++, 0.463508, 0.129893, 0.507652);
    addRGBColor(n++, 0.469640, 0.132245, 0.507809);
    addRGBColor(n++, 0.475780, 0.134577, 0.507921);
    addRGBColor(n++, 0.481929, 0.136891, 0.507989);
    addRGBColor(n++, 0.488088, 0.139186, 0.508011);
    addRGBColor(n++, 0.494258, 0.141462, 0.507988);
    addRGBColor(n++, 0.500438, 0.143719, 0.507920);
    addRGBColor(n++, 0.506629, 0.145958, 0.507806);
    addRGBColor(n++, 0.512831, 0.148179, 0.507648);
    addRGBColor(n++, 0.519045, 0.150383, 0.507443);
    addRGBColor(n++, 0.525270, 0.152569, 0.507192);
    addRGBColor(n++, 0.531507, 0.154739, 0.506895);
    addRGBColor(n++, 0.537755, 0.156894, 0.506551);
    addRGBColor(n++, 0.544015, 0.159033, 0.506159);
    addRGBColor(n++, 0.550287, 0.161158, 0.505719);
    addRGBColor(n++, 0.556571, 0.163269, 0.505230);
    addRGBColor(n++, 0.562866, 0.165368, 0.504692);
    addRGBColor(n++, 0.569172, 0.167454, 0.504105);
    addRGBColor(n++, 0.575490, 0.169530, 0.503466);
    addRGBColor(n++, 0.581819, 0.171596, 0.502777);
    addRGBColor(n++, 0.588158, 0.173652, 0.502035);
    addRGBColor(n++, 0.594508, 0.175701, 0.501241);
    addRGBColor(n++, 0.600868, 0.177743, 0.500394);
    addRGBColor(n++, 0.607238, 0.179779, 0.499492);
    addRGBColor(n++, 0.613617, 0.181811, 0.498536);
    addRGBColor(n++, 0.620005, 0.183840, 0.497524);
    addRGBColor(n++, 0.626401, 0.185867, 0.496456);
    addRGBColor(n++, 0.632805, 0.187893, 0.495332);
    addRGBColor(n++, 0.639216, 0.189921, 0.494150);
    addRGBColor(n++, 0.645633, 0.191952, 0.492910);
    addRGBColor(n++, 0.652056, 0.193986, 0.491611);
    addRGBColor(n++, 0.658483, 0.196027, 0.490253);
    addRGBColor(n++, 0.664915, 0.198075, 0.488836);
    addRGBColor(n++, 0.671349, 0.200133, 0.487358);
    addRGBColor(n++, 0.677786, 0.202203, 0.485819);
    addRGBColor(n++, 0.684224, 0.204286, 0.484219);
    addRGBColor(n++, 0.690661, 0.206384, 0.482558);
    addRGBColor(n++, 0.697098, 0.208501, 0.480835);
    addRGBColor(n++, 0.703532, 0.210638, 0.479049);
    addRGBColor(n++, 0.709962, 0.212797, 0.477201);
    addRGBColor(n++, 0.716387, 0.214982, 0.475290);
    addRGBColor(n++, 0.722805, 0.217194, 0.473316);
    addRGBColor(n++, 0.729216, 0.219437, 0.471279);
    addRGBColor(n++, 0.735616, 0.221713, 0.469180);
    addRGBColor(n++, 0.742004, 0.224025, 0.467018);
    addRGBColor(n++, 0.748378, 0.226377, 0.464794);
    addRGBColor(n++, 0.754737, 0.228772, 0.462509);
    addRGBColor(n++, 0.761077, 0.231214, 0.460162);
    addRGBColor(n++, 0.767398, 0.233705, 0.457755);
    addRGBColor(n++, 0.773695, 0.236249, 0.455289);
    addRGBColor(n++, 0.779968, 0.238851, 0.452765);
    addRGBColor(n++, 0.786212, 0.241514, 0.450184);
    addRGBColor(n++, 0.792427, 0.244242, 0.447543);
    addRGBColor(n++, 0.798608, 0.247040, 0.444848);
    addRGBColor(n++, 0.804752, 0.249911, 0.442102);
    addRGBColor(n++, 0.810855, 0.252861, 0.439305);
    addRGBColor(n++, 0.816914, 0.255895, 0.436461);
    addRGBColor(n++, 0.822926, 0.259016, 0.433573);
    addRGBColor(n++, 0.828886, 0.262229, 0.430644);
    addRGBColor(n++, 0.834791, 0.265540, 0.427671);
    addRGBColor(n++, 0.840636, 0.268953, 0.424666);
    addRGBColor(n++, 0.846416, 0.272473, 0.421631);
    addRGBColor(n++, 0.852126, 0.276106, 0.418573);
    addRGBColor(n++, 0.857763, 0.279857, 0.415496);
    addRGBColor(n++, 0.863320, 0.283729, 0.412403);
    addRGBColor(n++, 0.868793, 0.287728, 0.409303);
    addRGBColor(n++, 0.874176, 0.291859, 0.406205);
    addRGBColor(n++, 0.879464, 0.296125, 0.403118);
    addRGBColor(n++, 0.884651, 0.300530, 0.400047);
    addRGBColor(n++, 0.889731, 0.305079, 0.397002);
    addRGBColor(n++, 0.894700, 0.309773, 0.393995);
    addRGBColor(n++, 0.899552, 0.314616, 0.391037);
    addRGBColor(n++, 0.904281, 0.319610, 0.388137);
    addRGBColor(n++, 0.908884, 0.324755, 0.385308);
    addRGBColor(n++, 0.913354, 0.330052, 0.382563);
    addRGBColor(n++, 0.917689, 0.335500, 0.379915);
    addRGBColor(n++, 0.921884, 0.341098, 0.377376);
    addRGBColor(n++, 0.925937, 0.346844, 0.374959);
    addRGBColor(n++, 0.929845, 0.352734, 0.372677);
    addRGBColor(n++, 0.933606, 0.358764, 0.370541);
    addRGBColor(n++, 0.937221, 0.364929, 0.368567);
    addRGBColor(n++, 0.940687, 0.371224, 0.366762);
    addRGBColor(n++, 0.944006, 0.377643, 0.365136);
    addRGBColor(n++, 0.947180, 0.384178, 0.363701);
    addRGBColor(n++, 0.950210, 0.390820, 0.362468);
    addRGBColor(n++, 0.953099, 0.397563, 0.361438);
    addRGBColor(n++, 0.955849, 0.404400, 0.360619);
    addRGBColor(n++, 0.958464, 0.411324, 0.360014);
    addRGBColor(n++, 0.960949, 0.418323, 0.359630);
    addRGBColor(n++, 0.963310, 0.425390, 0.359469);
    addRGBColor(n++, 0.965549, 0.432519, 0.359529);
    addRGBColor(n++, 0.967671, 0.439703, 0.359810);
    addRGBColor(n++, 0.969680, 0.446936, 0.360311);
    addRGBColor(n++, 0.971582, 0.454210, 0.361030);
    addRGBColor(n++, 0.973381, 0.461520, 0.361965);
    addRGBColor(n++, 0.975082, 0.468861, 0.363111);
    addRGBColor(n++, 0.976690, 0.476226, 0.364466);
    addRGBColor(n++, 0.978210, 0.483612, 0.366025);
    addRGBColor(n++, 0.979645, 0.491014, 0.367783);
    addRGBColor(n++, 0.981000, 0.498428, 0.369734);
    addRGBColor(n++, 0.982279, 0.505851, 0.371874);
    addRGBColor(n++, 0.983485, 0.513280, 0.374198);
    addRGBColor(n++, 0.984622, 0.520713, 0.376698);
    addRGBColor(n++, 0.985693, 0.528148, 0.379371);
    addRGBColor(n++, 0.986700, 0.535582, 0.382210);
    addRGBColor(n++, 0.987646, 0.543015, 0.385210);
    addRGBColor(n++, 0.988533, 0.550446, 0.388365);
    addRGBColor(n++, 0.989363, 0.557873, 0.391671);
    addRGBColor(n++, 0.990138, 0.565296, 0.395122);
    addRGBColor(n++, 0.990871, 0.572706, 0.398714);
    addRGBColor(n++, 0.991558, 0.580107, 0.402441);
    addRGBColor(n++, 0.992196, 0.587502, 0.406299);
    addRGBColor(n++, 0.992785, 0.594891, 0.410283);
    addRGBColor(n++, 0.993326, 0.602275, 0.414390);
    addRGBColor(n++, 0.993834, 0.609644, 0.418613);
    addRGBColor(n++, 0.994309, 0.616999, 0.422950);
    addRGBColor(n++, 0.994738, 0.624350, 0.427397);
    addRGBColor(n++, 0.995122, 0.631696, 0.431951);
    addRGBColor(n++, 0.995480, 0.639027, 0.436607);
    addRGBColor(n++, 0.995810, 0.646344, 0.441361);
    addRGBColor(n++, 0.996096, 0.653659, 0.446213);
    addRGBColor(n++, 0.996341, 0.660969, 0.451160);
    addRGBColor(n++, 0.996580, 0.668256, 0.456192);
    addRGBColor(n++, 0.996775, 0.675541, 0.461314);
    addRGBColor(n++, 0.996925, 0.682828, 0.466526);
    addRGBColor(n++, 0.997077, 0.690088, 0.471811);
    addRGBColor(n++, 0.997186, 0.697349, 0.477182);
    addRGBColor(n++, 0.997254, 0.704611, 0.482635);
    addRGBColor(n++, 0.997325, 0.711848, 0.488154);
    addRGBColor(n++, 0.997351, 0.719089, 0.493755);
    addRGBColor(n++, 0.997351, 0.726324, 0.499428);
    addRGBColor(n++, 0.997341, 0.733545, 0.505167);
    addRGBColor(n++, 0.997285, 0.740772, 0.510983);
    addRGBColor(n++, 0.997228, 0.747981, 0.516859);
    addRGBColor(n++, 0.997138, 0.755190, 0.522806);
    addRGBColor(n++, 0.997019, 0.762398, 0.528821);
    addRGBColor(n++, 0.996898, 0.769591, 0.534892);
    addRGBColor(n++, 0.996727, 0.776795, 0.541039);
    addRGBColor(n++, 0.996571, 0.783977, 0.547233);
    addRGBColor(n++, 0.996369, 0.791167, 0.553499);
    addRGBColor(n++, 0.996162, 0.798348, 0.559820);
    addRGBColor(n++, 0.995932, 0.805527, 0.566202);
    addRGBColor(n++, 0.995680, 0.812706, 0.572645);
    addRGBColor(n++, 0.995424, 0.819875, 0.579140);
    addRGBColor(n++, 0.995131, 0.827052, 0.585701);
    addRGBColor(n++, 0.994851, 0.834213, 0.592307);
    addRGBColor(n++, 0.994524, 0.841387, 0.598983);
    addRGBColor(n++, 0.994222, 0.848540, 0.605696);
    addRGBColor(n++, 0.993866, 0.855711, 0.612482);
    addRGBColor(n++, 0.993545, 0.862859, 0.619299);
    addRGBColor(n++, 0.993170, 0.870024, 0.626189);
    addRGBColor(n++, 0.992831, 0.877168, 0.633109);
    addRGBColor(n++, 0.992440, 0.884330, 0.640099);
    addRGBColor(n++, 0.992089, 0.891470, 0.647116);
    addRGBColor(n++, 0.991688, 0.898627, 0.654202);
    addRGBColor(n++, 0.991332, 0.905763, 0.661309);
    addRGBColor(n++, 0.990930, 0.912915, 0.668481);
    addRGBColor(n++, 0.990570, 0.920049, 0.675675);
    addRGBColor(n++, 0.990175, 0.927196, 0.682926);
    addRGBColor(n++, 0.989815, 0.934329, 0.690198);
    addRGBColor(n++, 0.989434, 0.941470, 0.697519);
    addRGBColor(n++, 0.989077, 0.948604, 0.704863);
    addRGBColor(n++, 0.988717, 0.955742, 0.712242);
    addRGBColor(n++, 0.988367, 0.962878, 0.719649);
    addRGBColor(n++, 0.988033, 0.970012, 0.727077);
    addRGBColor(n++, 0.987691, 0.977154, 0.734536);
    addRGBColor(n++, 0.987387, 0.984288, 0.742002);
    addRGBColor(n++, 0.987053, 0.991438, 0.749504);
  }
};

class CQChartsPaletteMoreland : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteMoreland() {
    addRGBColor(0         , 0.2298057  , 0.298717966, 0.753683153);
    addRGBColor(0.00390625, 0.234299935, 0.305559204, 0.759874796);
    addRGBColor(0.0078125 , 0.238810063, 0.312388385, 0.766005866);
    addRGBColor(0.01171875, 0.243336663, 0.319205292, 0.772075394);
    addRGBColor(0.015625  , 0.247880265, 0.326009656, 0.778082421);
    addRGBColor(0.01953125, 0.25244136 , 0.332801165, 0.784026001);
    addRGBColor(0.0234375 , 0.257020396, 0.339579464, 0.789905199);
    addRGBColor(0.02734375, 0.261617779, 0.346344164, 0.79571909 );
    addRGBColor(0.03125   , 0.26623388 , 0.353094838, 0.801466763);
    addRGBColor(0.03515625, 0.270869029, 0.359831032, 0.807147315);
    addRGBColor(0.0390625 , 0.275523523, 0.36655226 , 0.812759858);
    addRGBColor(0.04296875, 0.28019762 , 0.373258014, 0.818303516);
    addRGBColor(0.046875  , 0.284891546, 0.379947761, 0.823777422);
    addRGBColor(0.05078125, 0.289605495, 0.386620945, 0.829180725);
    addRGBColor(0.0546875 , 0.294339624, 0.393276993, 0.834512584);
    addRGBColor(0.05859375, 0.299094064, 0.399915313, 0.839772171);
    addRGBColor(0.0625    , 0.30386891 , 0.406535296, 0.84495867 );
    addRGBColor(0.06640625, 0.308664231, 0.413136319, 0.850071279);
    addRGBColor(0.0703125 , 0.313480065, 0.419717745, 0.855109207);
    addRGBColor(0.07421875, 0.318316422, 0.426278924, 0.860071679);
    addRGBColor(0.078125  , 0.323173283, 0.432819194, 0.864957929);
    addRGBColor(0.08203125, 0.328050603, 0.439337884, 0.869767207);
    addRGBColor(0.0859375 , 0.332948312, 0.445834313, 0.874498775);
    addRGBColor(0.08984375, 0.337866311, 0.45230779 , 0.87915191 );
    addRGBColor(0.09375   , 0.342804478, 0.458757618, 0.883725899);
    addRGBColor(0.09765625, 0.347762667, 0.465183092, 0.888220047);
    addRGBColor(0.1015625 , 0.352740705, 0.471583499, 0.892633669);
    addRGBColor(0.10546875, 0.357738399, 0.477958123, 0.896966095);
    addRGBColor(0.109375  , 0.362755532, 0.484306241, 0.90121667 );
    addRGBColor(0.11328125, 0.367791863, 0.490627125, 0.905384751);
    addRGBColor(0.1171875 , 0.372847134, 0.496920043, 0.909469711);
    addRGBColor(0.12109375, 0.37792106 , 0.503184261, 0.913470934);
    addRGBColor(0.125     , 0.38301334 , 0.50941904 , 0.917387822);
    addRGBColor(0.12890625, 0.38812365 , 0.515623638, 0.921219788);
    addRGBColor(0.1328125 , 0.39325165 , 0.521797312, 0.924966262);
    addRGBColor(0.13671875, 0.398396976, 0.527939316, 0.928626686);
    addRGBColor(0.140625  , 0.40355925 , 0.534048902, 0.932200518);
    addRGBColor(0.14453125, 0.408738074, 0.540125323, 0.93568723 );
    addRGBColor(0.1484375 , 0.413933033, 0.546167829, 0.939086309);
    addRGBColor(0.15234375, 0.419143694, 0.552175668, 0.942397257);
    addRGBColor(0.15625   , 0.424369608, 0.558148092, 0.945619588);
    addRGBColor(0.16015625, 0.429610311, 0.564084349, 0.948752835);
    addRGBColor(0.1640625 , 0.434865321, 0.56998369 , 0.951796543);
    addRGBColor(0.16796875, 0.440134144, 0.575845364, 0.954750272);
    addRGBColor(0.171875  , 0.445416268, 0.581668623, 0.957613599);
    addRGBColor(0.17578125, 0.450711169, 0.587452719, 0.960386113);
    addRGBColor(0.1796875 , 0.456018308, 0.593196905, 0.96306742 );
    addRGBColor(0.18359375, 0.461337134, 0.598900436, 0.96565714 );
    addRGBColor(0.1875    , 0.46666708 , 0.604562568, 0.968154911);
    addRGBColor(0.19140625, 0.472007569, 0.61018256 , 0.970560381);
    addRGBColor(0.1953125 , 0.477358011, 0.615759672, 0.972873218);
    addRGBColor(0.19921875, 0.482717804, 0.621293167, 0.975093102);
    addRGBColor(0.203125  , 0.488086336, 0.626782311, 0.97721973 );
    addRGBColor(0.20703125, 0.493462982, 0.632226371, 0.979252813);
    addRGBColor(0.2109375 , 0.498847107, 0.637624618, 0.981192078);
    addRGBColor(0.21484375, 0.504238066, 0.642976326, 0.983037268);
    addRGBColor(0.21875   , 0.509635204, 0.648280772, 0.98478814 );
    addRGBColor(0.22265625, 0.515037856, 0.653537236, 0.986444467);
    addRGBColor(0.2265625 , 0.520445349, 0.658745003, 0.988006036);
    addRGBColor(0.23046875, 0.525857   , 0.66390336 , 0.989472652);
    addRGBColor(0.234375  , 0.531272118, 0.669011598, 0.990844132);
    addRGBColor(0.23828125, 0.536690004, 0.674069012, 0.99212031 );
    addRGBColor(0.2421875 , 0.542109949, 0.679074903, 0.993301037);
    addRGBColor(0.24609375, 0.54753124 , 0.684028574, 0.994386177);
    addRGBColor(0.25      , 0.552953156, 0.688929332, 0.995375608);
    addRGBColor(0.25390625, 0.558374965, 0.693776492, 0.996269227);
    addRGBColor(0.2578125 , 0.563795935, 0.698569369, 0.997066945);
    addRGBColor(0.26171875, 0.569215322, 0.703307287, 0.997768685);
    addRGBColor(0.265625  , 0.574632379, 0.707989572, 0.99837439 );
    addRGBColor(0.26953125, 0.580046354, 0.712615557, 0.998884016);
    addRGBColor(0.2734375 , 0.585456486, 0.717184578, 0.999297533);
    addRGBColor(0.27734375, 0.590862011, 0.721695979, 0.999614929);
    addRGBColor(0.28125   , 0.596262162, 0.726149107, 0.999836203);
    addRGBColor(0.28515625, 0.601656165, 0.730543315, 0.999961374);
    addRGBColor(0.2890625 , 0.607043242, 0.734877964, 0.999990472);
    addRGBColor(0.29296875, 0.61242261 , 0.739152418, 0.999923544);
    addRGBColor(0.296875  , 0.617793485, 0.743366047, 0.999760652);
    addRGBColor(0.30078125, 0.623155076, 0.747518228, 0.999501871);
    addRGBColor(0.3046875 , 0.628506592, 0.751608345, 0.999147293);
    addRGBColor(0.30859375, 0.633847237, 0.755635786, 0.998697024);
    addRGBColor(0.3125    , 0.639176211, 0.759599947, 0.998151185);
    addRGBColor(0.31640625, 0.644492714, 0.763500228, 0.99750991 );
    addRGBColor(0.3203125 , 0.649795942, 0.767336039, 0.996773351);
    addRGBColor(0.32421875, 0.655085089, 0.771106793, 0.995941671);
    addRGBColor(0.328125  , 0.660359348, 0.774811913, 0.995015049);
    addRGBColor(0.33203125, 0.665617908, 0.778450826, 0.993993679);
    addRGBColor(0.3359375 , 0.670859959, 0.782022968, 0.992877768);
    addRGBColor(0.33984375, 0.676084688, 0.78552778 , 0.991667539);
    addRGBColor(0.34375   , 0.681291281, 0.788964712, 0.990363227);
    addRGBColor(0.34765625, 0.686478925, 0.792333219, 0.988965083);
    addRGBColor(0.3515625 , 0.691646803, 0.795632765, 0.987473371);
    addRGBColor(0.35546875, 0.696794099, 0.798862821, 0.985888369);
    addRGBColor(0.359375  , 0.701919999, 0.802022864, 0.984210369);
    addRGBColor(0.36328125, 0.707023684, 0.805112381, 0.982439677);
    addRGBColor(0.3671875 , 0.712104339, 0.808130864, 0.980576612);
    addRGBColor(0.37109375, 0.717161148, 0.811077814, 0.978621507);
    addRGBColor(0.375     , 0.722193294, 0.813952739, 0.976574709);
    addRGBColor(0.37890625, 0.727199962, 0.816755156, 0.974436577);
    addRGBColor(0.3828125 , 0.732180337, 0.81948459 , 0.972207484);
    addRGBColor(0.38671875, 0.737133606, 0.82214057 , 0.969887816);
    addRGBColor(0.390625  , 0.742058956, 0.824722639, 0.967477972);
    addRGBColor(0.39453125, 0.746955574, 0.827230344, 0.964978364);
    addRGBColor(0.3984375 , 0.751822652, 0.829663241, 0.962389418);
    addRGBColor(0.40234375, 0.756659379, 0.832020895, 0.959711569);
    addRGBColor(0.40625   , 0.761464949, 0.834302879, 0.956945269);
    addRGBColor(0.41015625, 0.766238556, 0.836508774, 0.95409098 );
    addRGBColor(0.4140625 , 0.770979397, 0.838638169, 0.951149176);
    addRGBColor(0.41796875, 0.775686671, 0.840690662, 0.948120345);
    addRGBColor(0.421875  , 0.780359577, 0.842665861, 0.945004985);
    addRGBColor(0.42578125, 0.78499732 , 0.84456338 , 0.941803607);
    addRGBColor(0.4296875 , 0.789599105, 0.846382843, 0.938516733);
    addRGBColor(0.43359375, 0.79416414 , 0.848123884, 0.935144898);
    addRGBColor(0.4375    , 0.798691636, 0.849786142, 0.931688648);
    addRGBColor(0.44140625, 0.803180808, 0.85136927 , 0.928148539);
    addRGBColor(0.4453125 , 0.807630872, 0.852872925, 0.92452514 );
    addRGBColor(0.44921875, 0.812041048, 0.854296776, 0.92081903 );
    addRGBColor(0.453125  , 0.81641056 , 0.855640499, 0.917030798);
    addRGBColor(0.45703125, 0.820738635, 0.856903782, 0.913161047);
    addRGBColor(0.4609375 , 0.825024503, 0.85808632 , 0.909210387);
    addRGBColor(0.46484375, 0.829267397, 0.859187816, 0.90517944 );
    addRGBColor(0.46875   , 0.833466556, 0.860207984, 0.901068838);
    addRGBColor(0.47265625, 0.837621221, 0.861146547, 0.896879224);
    addRGBColor(0.4765625 , 0.841730637, 0.862003236, 0.892611249);
    addRGBColor(0.48046875, 0.845794055, 0.862777795, 0.888265576);
    addRGBColor(0.484375  , 0.849810727, 0.863469972, 0.883842876);
    addRGBColor(0.48828125, 0.853779913, 0.864079527, 0.87934383 );
    addRGBColor(0.4921875 , 0.857700874, 0.864606232, 0.874769128);
    addRGBColor(0.49609375, 0.861572878, 0.865049863, 0.870119469);
    addRGBColor(0.5       , 0.865395197, 0.86541021 , 0.865395561);
    addRGBColor(0.50390625, 0.86977749 , 0.863633958, 0.859948576);
    addRGBColor(0.5078125 , 0.874064226, 0.861776352, 0.854466231);
    addRGBColor(0.51171875, 0.878255583, 0.859837644, 0.848949435);
    addRGBColor(0.515625  , 0.882351728, 0.857818097, 0.843399101);
    addRGBColor(0.51953125, 0.886352818, 0.85571798 , 0.837816138);
    addRGBColor(0.5234375 , 0.890259   , 0.853537573, 0.832201453);
    addRGBColor(0.52734375, 0.89407041 , 0.851277164, 0.826555954);
    addRGBColor(0.53125   , 0.897787179, 0.848937047, 0.820880546);
    addRGBColor(0.53515625, 0.901409427, 0.846517528, 0.815176131);
    addRGBColor(0.5390625 , 0.904937269, 0.844018919, 0.809443611);
    addRGBColor(0.54296875, 0.908370816, 0.841441541, 0.803683885);
    addRGBColor(0.546875  , 0.911710171, 0.838785722, 0.79789785 );
    addRGBColor(0.55078125, 0.914955433, 0.836051799, 0.792086401);
    addRGBColor(0.5546875 , 0.918106696, 0.833240115, 0.786250429);
    addRGBColor(0.55859375, 0.921164054, 0.830351023, 0.780390824);
    addRGBColor(0.5625    , 0.924127593, 0.827384882, 0.774508472);
    addRGBColor(0.56640625, 0.926997401, 0.824342058, 0.768604257);
    addRGBColor(0.5703125 , 0.929773562, 0.821222926, 0.76267906 );
    addRGBColor(0.57421875, 0.932456159, 0.818027865, 0.756733758);
    addRGBColor(0.578125  , 0.935045272, 0.814757264, 0.750769226);
    addRGBColor(0.58203125, 0.937540984, 0.811411517, 0.744786333);
    addRGBColor(0.5859375 , 0.939943375, 0.807991025, 0.738785947);
    addRGBColor(0.58984375, 0.942252526, 0.804496196, 0.732768931);
    addRGBColor(0.59375   , 0.944468518, 0.800927443, 0.726736146);
    addRGBColor(0.59765625, 0.946591434, 0.797285187, 0.720688446);
    addRGBColor(0.6015625 , 0.948621357, 0.793569853, 0.714626683);
    addRGBColor(0.60546875, 0.950558373, 0.789781872, 0.708551706);
    addRGBColor(0.609375  , 0.952402567, 0.785921682, 0.702464356);
    addRGBColor(0.61328125, 0.954154029, 0.781989725, 0.696365473);
    addRGBColor(0.6171875 , 0.955812849, 0.777986449, 0.690255891);
    addRGBColor(0.62109375, 0.957379123, 0.773912305, 0.68413644 );
    addRGBColor(0.625     , 0.958852946, 0.769767752, 0.678007945);
    addRGBColor(0.62890625, 0.960234418, 0.765553251, 0.671871226);
    addRGBColor(0.6328125 , 0.961523642, 0.761269267, 0.665727098);
    addRGBColor(0.63671875, 0.962720725, 0.756916272, 0.659576372);
    addRGBColor(0.640625  , 0.963825777, 0.752494738, 0.653419853);
    addRGBColor(0.64453125, 0.964838913, 0.748005143, 0.647258341);
    addRGBColor(0.6484375 , 0.965760251, 0.743447967, 0.64109263 );
    addRGBColor(0.65234375, 0.966589914, 0.738823693, 0.634923509);
    addRGBColor(0.65625   , 0.96732803 , 0.734132809, 0.628751763);
    addRGBColor(0.66015625, 0.967974729, 0.729375802, 0.62257817 );
    addRGBColor(0.6640625 , 0.96853015 , 0.724553162, 0.616403502);
    addRGBColor(0.66796875, 0.968994435, 0.719665383, 0.610228525);
    addRGBColor(0.671875  , 0.969367729, 0.714712956, 0.604054002);
    addRGBColor(0.67578125, 0.969650186, 0.709696378, 0.597880686);
    addRGBColor(0.6796875 , 0.969841963, 0.704616143, 0.591709328);
    addRGBColor(0.68359375, 0.969943224, 0.699472746, 0.585540669);
    addRGBColor(0.6875    , 0.969954137, 0.694266682, 0.579375448);
    addRGBColor(0.69140625, 0.969874878, 0.688998447, 0.573214394);
    addRGBColor(0.6953125 , 0.969705626, 0.683668532, 0.567058232);
    addRGBColor(0.69921875, 0.96944657 , 0.678277431, 0.560907681);
    addRGBColor(0.703125  , 0.969097901, 0.672825633, 0.554763452);
    addRGBColor(0.70703125, 0.968659818, 0.667313624, 0.54862625 );
    addRGBColor(0.7109375 , 0.968132528, 0.661741889, 0.542496774);
    addRGBColor(0.71484375, 0.967516241, 0.656110908, 0.536375716);
    addRGBColor(0.71875   , 0.966811177, 0.650421156, 0.530263762);
    addRGBColor(0.72265625, 0.966017559, 0.644673104, 0.524161591);
    addRGBColor(0.7265625 , 0.965135621, 0.638867216, 0.518069875);
    addRGBColor(0.73046875, 0.964165599, 0.63300395 , 0.511989279);
    addRGBColor(0.734375  , 0.963107739, 0.627083758, 0.505920462);
    addRGBColor(0.73828125, 0.961962293, 0.621107082, 0.499864075);
    addRGBColor(0.7421875 , 0.960729521, 0.615074355, 0.493820764);
    addRGBColor(0.74609375, 0.959409687, 0.608986   , 0.487791167);
    addRGBColor(0.75      , 0.958003065, 0.602842431, 0.481775914);
    addRGBColor(0.75390625, 0.956509936, 0.596644046, 0.475775629);
    addRGBColor(0.7578125 , 0.954930586, 0.590391232, 0.46979093 );
    addRGBColor(0.76171875, 0.95326531 , 0.584084361, 0.463822426);
    addRGBColor(0.765625  , 0.951514411, 0.57772379 , 0.457870719);
    addRGBColor(0.76953125, 0.949678196, 0.571309856, 0.451936407);
    addRGBColor(0.7734375 , 0.947756983, 0.564842879, 0.446020077);
    addRGBColor(0.77734375, 0.945751096, 0.558323158, 0.440122312);
    addRGBColor(0.78125   , 0.943660866, 0.551750968, 0.434243684);
    addRGBColor(0.78515625, 0.941486631, 0.545126562, 0.428384763);
    addRGBColor(0.7890625 , 0.939228739, 0.538450165, 0.422546107);
    addRGBColor(0.79296875, 0.936887543, 0.531721972, 0.41672827 );
    addRGBColor(0.796875  , 0.934463404, 0.524942147, 0.410931798);
    addRGBColor(0.80078125, 0.931956691, 0.518110821, 0.40515723 );
    addRGBColor(0.8046875 , 0.929367782, 0.511228087, 0.399405096);
    addRGBColor(0.80859375, 0.92669706 , 0.504293997, 0.393675922);
    addRGBColor(0.8125    , 0.923944917, 0.49730856 , 0.387970225);
    addRGBColor(0.81640625, 0.921111753, 0.490271735, 0.382288516);
    addRGBColor(0.8203125 , 0.918197974, 0.483183431, 0.376631297);
    addRGBColor(0.82421875, 0.915203996, 0.476043498, 0.370999065);
    addRGBColor(0.828125  , 0.912130241, 0.468851724, 0.36539231 );
    addRGBColor(0.83203125, 0.908977139, 0.461607831, 0.359811513);
    addRGBColor(0.8359375 , 0.905745128, 0.454311462, 0.354257151);
    addRGBColor(0.83984375, 0.902434654, 0.446962183, 0.348729691);
    addRGBColor(0.84375   , 0.89904617 , 0.439559467, 0.343229596);
    addRGBColor(0.84765625, 0.895580136, 0.43210269 , 0.33775732 );
    addRGBColor(0.8515625 , 0.892037022, 0.424591118, 0.332313313);
    addRGBColor(0.85546875, 0.888417303, 0.417023898, 0.326898016);
    addRGBColor(0.859375  , 0.884721464, 0.409400045, 0.321511863);
    addRGBColor(0.86328125, 0.880949996, 0.401718425, 0.316155284);
    addRGBColor(0.8671875 , 0.877103399, 0.393977745, 0.310828702);
    addRGBColor(0.87109375, 0.873182178, 0.386176527, 0.305532531);
    addRGBColor(0.875     , 0.869186849, 0.378313092, 0.300267182);
    addRGBColor(0.87890625, 0.865117934, 0.370385535, 0.295033059);
    addRGBColor(0.8828125 , 0.860975962, 0.362391695, 0.289830559);
    addRGBColor(0.88671875, 0.85676147 , 0.354329127, 0.284660075);
    addRGBColor(0.890625  , 0.852475004, 0.346195061, 0.279521991);
    addRGBColor(0.89453125, 0.848117114, 0.337986361, 0.27441669 );
    addRGBColor(0.8984375 , 0.843688361, 0.329699471, 0.269344545);
    addRGBColor(0.90234375, 0.839189312, 0.32133036 , 0.264305927);
    addRGBColor(0.90625   , 0.834620542, 0.312874446, 0.259301199);
    addRGBColor(0.91015625, 0.829982631, 0.304326513, 0.254330723);
    addRGBColor(0.9140625 , 0.82527617 , 0.295680611, 0.249394851);
    addRGBColor(0.91796875, 0.820501754, 0.286929926, 0.244493934);
    addRGBColor(0.921875  , 0.815659988, 0.278066636, 0.239628318);
    addRGBColor(0.92578125, 0.810751482, 0.269081721, 0.234798343);
    addRGBColor(0.9296875 , 0.805776855, 0.259964733, 0.230004348);
    addRGBColor(0.93359375, 0.800736732, 0.250703507, 0.225246666);
    addRGBColor(0.9375    , 0.795631745, 0.24128379 , 0.220525627);
    addRGBColor(0.94140625, 0.790462533, 0.231688768, 0.215841558);
    addRGBColor(0.9453125 , 0.785229744, 0.221898442, 0.211194782);
    addRGBColor(0.94921875, 0.779934029, 0.211888813, 0.20658562 );
    addRGBColor(0.953125  , 0.774576051, 0.201630762, 0.202014392);
    addRGBColor(0.95703125, 0.769156474, 0.191088518, 0.197481414);
    addRGBColor(0.9609375 , 0.763675975, 0.180217488, 0.192987001);
    addRGBColor(0.96484375, 0.758135232, 0.168961101, 0.188531467);
    addRGBColor(0.96875   , 0.752534934, 0.157246067, 0.184115123);
    addRGBColor(0.97265625, 0.746875773, 0.144974956, 0.179738284);
    addRGBColor(0.9765625 , 0.741158452, 0.132014017, 0.175401259);
    addRGBColor(0.98046875, 0.735383675, 0.1181719  , 0.171104363);
    addRGBColor(0.984375  , 0.729552157, 0.103159409, 0.166847907);
    addRGBColor(0.98828125, 0.723664618, 0.086504694, 0.162632207);
    addRGBColor(0.9921875 , 0.717721782, 0.067344036, 0.158457578);
    addRGBColor(0.99609375, 0.711724383, 0.043755173, 0.154324339);
    addRGBColor(1         , 0.705673158, 0.01555616 , 0.150232812);
  }
};

class CQChartsPaletteOranges : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteOranges() {
    addDefinedColor(0.0, QColor("#FFF5EB"));
    addDefinedColor(1.0, QColor("#FEE6CE"));
    addDefinedColor(2.0, QColor("#FDD0A2"));
    addDefinedColor(3.0, QColor("#FDAE6B"));
    addDefinedColor(4.0, QColor("#FD8D3C"));
    addDefinedColor(5.0, QColor("#F16913"));
    addDefinedColor(6.0, QColor("#D94801"));
    addDefinedColor(7.0, QColor("#8C2D04"));
  }
};

class CQChartsPaletteOrrd : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteOrrd() {
    addDefinedColor(0.0, QColor("#FFF7EC"));
    addDefinedColor(1.0, QColor("#FEE8C8"));
    addDefinedColor(2.0, QColor("#FDD49E"));
    addDefinedColor(3.0, QColor("#FDBB84"));
    addDefinedColor(4.0, QColor("#FC8D59"));
    addDefinedColor(5.0, QColor("#EF6548"));
    addDefinedColor(6.0, QColor("#D7301F"));
    addDefinedColor(7.0, QColor("#990000"));
  }
};

class CQChartsPalettePaired : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePaired() {
    addDefinedColor(0.0, QColor("#A6CEE3"));
    addDefinedColor(1.0, QColor("#1F78B4"));
    addDefinedColor(2.0, QColor("#B2DF8A"));
    addDefinedColor(3.0, QColor("#33A02C"));
    addDefinedColor(4.0, QColor("#FB9A99"));
    addDefinedColor(5.0, QColor("#E31A1C"));
    addDefinedColor(6.0, QColor("#FDBF6F"));
    addDefinedColor(7.0, QColor("#FF7F00"));
  }
};

class CQChartsPaletteParula : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteParula() {
    addDefinedColor(0.0, QColor("#352a87"));
    addDefinedColor(1.0, QColor("#0363e1"));
    addDefinedColor(2.0, QColor("#1485d4"));
    addDefinedColor(3.0, QColor("#06a7c6"));
    addDefinedColor(4.0, QColor("#38b99e"));
    addDefinedColor(5.0, QColor("#92bf73"));
    addDefinedColor(6.0, QColor("#d9ba56"));
    addDefinedColor(7.0, QColor("#fcce2e"));
    addDefinedColor(8.0, QColor("#f9fb0e"));
  }
};

class CQChartsPalettePastel1 : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePastel1() {
    addDefinedColor(0.0, QColor("#FBB4AE"));
    addDefinedColor(1.0, QColor("#B3CDE3"));
    addDefinedColor(2.0, QColor("#CCEBC5"));
    addDefinedColor(3.0, QColor("#DECBE4"));
    addDefinedColor(4.0, QColor("#FED9A6"));
    addDefinedColor(5.0, QColor("#FFFFCC"));
    addDefinedColor(6.0, QColor("#E5D8BD"));
    addDefinedColor(7.0, QColor("#FDDAEC"));
  }
};

class CQChartsPalettePastel2 : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePastel2() {
    addDefinedColor(0.0, QColor("#B3E2CD"));
    addDefinedColor(1.0, QColor("#FDCDAC"));
    addDefinedColor(2.0, QColor("#CDB5E8"));
    addDefinedColor(3.0, QColor("#F4CAE4"));
    addDefinedColor(4.0, QColor("#D6F5C9"));
    addDefinedColor(5.0, QColor("#FFF2AE"));
    addDefinedColor(6.0, QColor("#F1E2CC"));
    addDefinedColor(7.0, QColor("#CCCCCC"));
  }
};

class CQChartsPalettePiyg : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePiyg() {
    addDefinedColor(0.0, QColor("#C51B7D"));
    addDefinedColor(1.0, QColor("#DE77AE"));
    addDefinedColor(2.0, QColor("#F1B6DA"));
    addDefinedColor(3.0, QColor("#FDE0EF"));
    addDefinedColor(4.0, QColor("#E6F5D0"));
    addDefinedColor(5.0, QColor("#B8E186"));
    addDefinedColor(6.0, QColor("#7FBC41"));
    addDefinedColor(7.0, QColor("#4D9221"));
  }
};

class CQChartsPalettePlasma : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePlasma() {
    int n = 0;

    addRGBColor(n++, 0.050383, 0.029803, 0.527975);
    addRGBColor(n++, 0.063536, 0.028426, 0.533124);
    addRGBColor(n++, 0.075353, 0.027206, 0.538007);
    addRGBColor(n++, 0.086222, 0.026125, 0.542658);
    addRGBColor(n++, 0.096379, 0.025165, 0.547103);
    addRGBColor(n++, 0.105980, 0.024309, 0.551368);
    addRGBColor(n++, 0.115124, 0.023556, 0.555468);
    addRGBColor(n++, 0.123903, 0.022878, 0.559423);
    addRGBColor(n++, 0.132381, 0.022258, 0.563250);
    addRGBColor(n++, 0.140603, 0.021687, 0.566959);
    addRGBColor(n++, 0.148607, 0.021154, 0.570562);
    addRGBColor(n++, 0.156421, 0.020651, 0.574065);
    addRGBColor(n++, 0.164070, 0.020171, 0.577478);
    addRGBColor(n++, 0.171574, 0.019706, 0.580806);
    addRGBColor(n++, 0.178950, 0.019252, 0.584054);
    addRGBColor(n++, 0.186213, 0.018803, 0.587228);
    addRGBColor(n++, 0.193374, 0.018354, 0.590330);
    addRGBColor(n++, 0.200445, 0.017902, 0.593364);
    addRGBColor(n++, 0.207435, 0.017442, 0.596333);
    addRGBColor(n++, 0.214350, 0.016973, 0.599239);
    addRGBColor(n++, 0.221197, 0.016497, 0.602083);
    addRGBColor(n++, 0.227983, 0.016007, 0.604867);
    addRGBColor(n++, 0.234715, 0.015502, 0.607592);
    addRGBColor(n++, 0.241396, 0.014979, 0.610259);
    addRGBColor(n++, 0.248032, 0.014439, 0.612868);
    addRGBColor(n++, 0.254627, 0.013882, 0.615419);
    addRGBColor(n++, 0.261183, 0.013308, 0.617911);
    addRGBColor(n++, 0.267703, 0.012716, 0.620346);
    addRGBColor(n++, 0.274191, 0.012109, 0.622722);
    addRGBColor(n++, 0.280648, 0.011488, 0.625038);
    addRGBColor(n++, 0.287076, 0.010855, 0.627295);
    addRGBColor(n++, 0.293478, 0.010213, 0.629490);
    addRGBColor(n++, 0.299855, 0.009561, 0.631624);
    addRGBColor(n++, 0.306210, 0.008902, 0.633694);
    addRGBColor(n++, 0.312543, 0.008239, 0.635700);
    addRGBColor(n++, 0.318856, 0.007576, 0.637640);
    addRGBColor(n++, 0.325150, 0.006915, 0.639512);
    addRGBColor(n++, 0.331426, 0.006261, 0.641316);
    addRGBColor(n++, 0.337683, 0.005618, 0.643049);
    addRGBColor(n++, 0.343925, 0.004991, 0.644710);
    addRGBColor(n++, 0.350150, 0.004382, 0.646298);
    addRGBColor(n++, 0.356359, 0.003798, 0.647810);
    addRGBColor(n++, 0.362553, 0.003243, 0.649245);
    addRGBColor(n++, 0.368733, 0.002724, 0.650601);
    addRGBColor(n++, 0.374897, 0.002245, 0.651876);
    addRGBColor(n++, 0.381047, 0.001814, 0.653068);
    addRGBColor(n++, 0.387183, 0.001434, 0.654177);
    addRGBColor(n++, 0.393304, 0.001114, 0.655199);
    addRGBColor(n++, 0.399411, 0.000859, 0.656133);
    addRGBColor(n++, 0.405503, 0.000678, 0.656977);
    addRGBColor(n++, 0.411580, 0.000577, 0.657730);
    addRGBColor(n++, 0.417642, 0.000564, 0.658390);
    addRGBColor(n++, 0.423689, 0.000646, 0.658956);
    addRGBColor(n++, 0.429719, 0.000831, 0.659425);
    addRGBColor(n++, 0.435734, 0.001127, 0.659797);
    addRGBColor(n++, 0.441732, 0.001540, 0.660069);
    addRGBColor(n++, 0.447714, 0.002080, 0.660240);
    addRGBColor(n++, 0.453677, 0.002755, 0.660310);
    addRGBColor(n++, 0.459623, 0.003574, 0.660277);
    addRGBColor(n++, 0.465550, 0.004545, 0.660139);
    addRGBColor(n++, 0.471457, 0.005678, 0.659897);
    addRGBColor(n++, 0.477344, 0.006980, 0.659549);
    addRGBColor(n++, 0.483210, 0.008460, 0.659095);
    addRGBColor(n++, 0.489055, 0.010127, 0.658534);
    addRGBColor(n++, 0.494877, 0.011990, 0.657865);
    addRGBColor(n++, 0.500678, 0.014055, 0.657088);
    addRGBColor(n++, 0.506454, 0.016333, 0.656202);
    addRGBColor(n++, 0.512206, 0.018833, 0.655209);
    addRGBColor(n++, 0.517933, 0.021563, 0.654109);
    addRGBColor(n++, 0.523633, 0.024532, 0.652901);
    addRGBColor(n++, 0.529306, 0.027747, 0.651586);
    addRGBColor(n++, 0.534952, 0.031217, 0.650165);
    addRGBColor(n++, 0.540570, 0.034950, 0.648640);
    addRGBColor(n++, 0.546157, 0.038954, 0.647010);
    addRGBColor(n++, 0.551715, 0.043136, 0.645277);
    addRGBColor(n++, 0.557243, 0.047331, 0.643443);
    addRGBColor(n++, 0.562738, 0.051545, 0.641509);
    addRGBColor(n++, 0.568201, 0.055778, 0.639477);
    addRGBColor(n++, 0.573632, 0.060028, 0.637349);
    addRGBColor(n++, 0.579029, 0.064296, 0.635126);
    addRGBColor(n++, 0.584391, 0.068579, 0.632812);
    addRGBColor(n++, 0.589719, 0.072878, 0.630408);
    addRGBColor(n++, 0.595011, 0.077190, 0.627917);
    addRGBColor(n++, 0.600266, 0.081516, 0.625342);
    addRGBColor(n++, 0.605485, 0.085854, 0.622686);
    addRGBColor(n++, 0.610667, 0.090204, 0.619951);
    addRGBColor(n++, 0.615812, 0.094564, 0.617140);
    addRGBColor(n++, 0.620919, 0.098934, 0.614257);
    addRGBColor(n++, 0.625987, 0.103312, 0.611305);
    addRGBColor(n++, 0.631017, 0.107699, 0.608287);
    addRGBColor(n++, 0.636008, 0.112092, 0.605205);
    addRGBColor(n++, 0.640959, 0.116492, 0.602065);
    addRGBColor(n++, 0.645872, 0.120898, 0.598867);
    addRGBColor(n++, 0.650746, 0.125309, 0.595617);
    addRGBColor(n++, 0.655580, 0.129725, 0.592317);
    addRGBColor(n++, 0.660374, 0.134144, 0.588971);
    addRGBColor(n++, 0.665129, 0.138566, 0.585582);
    addRGBColor(n++, 0.669845, 0.142992, 0.582154);
    addRGBColor(n++, 0.674522, 0.147419, 0.578688);
    addRGBColor(n++, 0.679160, 0.151848, 0.575189);
    addRGBColor(n++, 0.683758, 0.156278, 0.571660);
    addRGBColor(n++, 0.688318, 0.160709, 0.568103);
    addRGBColor(n++, 0.692840, 0.165141, 0.564522);
    addRGBColor(n++, 0.697324, 0.169573, 0.560919);
    addRGBColor(n++, 0.701769, 0.174005, 0.557296);
    addRGBColor(n++, 0.706178, 0.178437, 0.553657);
    addRGBColor(n++, 0.710549, 0.182868, 0.550004);
    addRGBColor(n++, 0.714883, 0.187299, 0.546338);
    addRGBColor(n++, 0.719181, 0.191729, 0.542663);
    addRGBColor(n++, 0.723444, 0.196158, 0.538981);
    addRGBColor(n++, 0.727670, 0.200586, 0.535293);
    addRGBColor(n++, 0.731862, 0.205013, 0.531601);
    addRGBColor(n++, 0.736019, 0.209439, 0.527908);
    addRGBColor(n++, 0.740143, 0.213864, 0.524216);
    addRGBColor(n++, 0.744232, 0.218288, 0.520524);
    addRGBColor(n++, 0.748289, 0.222711, 0.516834);
    addRGBColor(n++, 0.752312, 0.227133, 0.513149);
    addRGBColor(n++, 0.756304, 0.231555, 0.509468);
    addRGBColor(n++, 0.760264, 0.235976, 0.505794);
    addRGBColor(n++, 0.764193, 0.240396, 0.502126);
    addRGBColor(n++, 0.768090, 0.244817, 0.498465);
    addRGBColor(n++, 0.771958, 0.249237, 0.494813);
    addRGBColor(n++, 0.775796, 0.253658, 0.491171);
    addRGBColor(n++, 0.779604, 0.258078, 0.487539);
    addRGBColor(n++, 0.783383, 0.262500, 0.483918);
    addRGBColor(n++, 0.787133, 0.266922, 0.480307);
    addRGBColor(n++, 0.790855, 0.271345, 0.476706);
    addRGBColor(n++, 0.794549, 0.275770, 0.473117);
    addRGBColor(n++, 0.798216, 0.280197, 0.469538);
    addRGBColor(n++, 0.801855, 0.284626, 0.465971);
    addRGBColor(n++, 0.805467, 0.289057, 0.462415);
    addRGBColor(n++, 0.809052, 0.293491, 0.458870);
    addRGBColor(n++, 0.812612, 0.297928, 0.455338);
    addRGBColor(n++, 0.816144, 0.302368, 0.451816);
    addRGBColor(n++, 0.819651, 0.306812, 0.448306);
    addRGBColor(n++, 0.823132, 0.311261, 0.444806);
    addRGBColor(n++, 0.826588, 0.315714, 0.441316);
    addRGBColor(n++, 0.830018, 0.320172, 0.437836);
    addRGBColor(n++, 0.833422, 0.324635, 0.434366);
    addRGBColor(n++, 0.836801, 0.329105, 0.430905);
    addRGBColor(n++, 0.840155, 0.333580, 0.427455);
    addRGBColor(n++, 0.843484, 0.338062, 0.424013);
    addRGBColor(n++, 0.846788, 0.342551, 0.420579);
    addRGBColor(n++, 0.850066, 0.347048, 0.417153);
    addRGBColor(n++, 0.853319, 0.351553, 0.413734);
    addRGBColor(n++, 0.856547, 0.356066, 0.410322);
    addRGBColor(n++, 0.859750, 0.360588, 0.406917);
    addRGBColor(n++, 0.862927, 0.365119, 0.403519);
    addRGBColor(n++, 0.866078, 0.369660, 0.400126);
    addRGBColor(n++, 0.869203, 0.374212, 0.396738);
    addRGBColor(n++, 0.872303, 0.378774, 0.393355);
    addRGBColor(n++, 0.875376, 0.383347, 0.389976);
    addRGBColor(n++, 0.878423, 0.387932, 0.386600);
    addRGBColor(n++, 0.881443, 0.392529, 0.383229);
    addRGBColor(n++, 0.884436, 0.397139, 0.379860);
    addRGBColor(n++, 0.887402, 0.401762, 0.376494);
    addRGBColor(n++, 0.890340, 0.406398, 0.373130);
    addRGBColor(n++, 0.893250, 0.411048, 0.369768);
    addRGBColor(n++, 0.896131, 0.415712, 0.366407);
    addRGBColor(n++, 0.898984, 0.420392, 0.363047);
    addRGBColor(n++, 0.901807, 0.425087, 0.359688);
    addRGBColor(n++, 0.904601, 0.429797, 0.356329);
    addRGBColor(n++, 0.907365, 0.434524, 0.352970);
    addRGBColor(n++, 0.910098, 0.439268, 0.349610);
    addRGBColor(n++, 0.912800, 0.444029, 0.346251);
    addRGBColor(n++, 0.915471, 0.448807, 0.342890);
    addRGBColor(n++, 0.918109, 0.453603, 0.339529);
    addRGBColor(n++, 0.920714, 0.458417, 0.336166);
    addRGBColor(n++, 0.923287, 0.463251, 0.332801);
    addRGBColor(n++, 0.925825, 0.468103, 0.329435);
    addRGBColor(n++, 0.928329, 0.472975, 0.326067);
    addRGBColor(n++, 0.930798, 0.477867, 0.322697);
    addRGBColor(n++, 0.933232, 0.482780, 0.319325);
    addRGBColor(n++, 0.935630, 0.487712, 0.315952);
    addRGBColor(n++, 0.937990, 0.492667, 0.312575);
    addRGBColor(n++, 0.940313, 0.497642, 0.309197);
    addRGBColor(n++, 0.942598, 0.502639, 0.305816);
    addRGBColor(n++, 0.944844, 0.507658, 0.302433);
    addRGBColor(n++, 0.947051, 0.512699, 0.299049);
    addRGBColor(n++, 0.949217, 0.517763, 0.295662);
    addRGBColor(n++, 0.951344, 0.522850, 0.292275);
    addRGBColor(n++, 0.953428, 0.527960, 0.288883);
    addRGBColor(n++, 0.955470, 0.533093, 0.285490);
    addRGBColor(n++, 0.957469, 0.538250, 0.282096);
    addRGBColor(n++, 0.959424, 0.543431, 0.278701);
    addRGBColor(n++, 0.961336, 0.548636, 0.275305);
    addRGBColor(n++, 0.963203, 0.553865, 0.271909);
    addRGBColor(n++, 0.965024, 0.559118, 0.268513);
    addRGBColor(n++, 0.966798, 0.564396, 0.265118);
    addRGBColor(n++, 0.968526, 0.569700, 0.261721);
    addRGBColor(n++, 0.970205, 0.575028, 0.258325);
    addRGBColor(n++, 0.971835, 0.580382, 0.254931);
    addRGBColor(n++, 0.973416, 0.585761, 0.251540);
    addRGBColor(n++, 0.974947, 0.591165, 0.248151);
    addRGBColor(n++, 0.976428, 0.596595, 0.244767);
    addRGBColor(n++, 0.977856, 0.602051, 0.241387);
    addRGBColor(n++, 0.979233, 0.607532, 0.238013);
    addRGBColor(n++, 0.980556, 0.613039, 0.234646);
    addRGBColor(n++, 0.981826, 0.618572, 0.231287);
    addRGBColor(n++, 0.983041, 0.624131, 0.227937);
    addRGBColor(n++, 0.984199, 0.629718, 0.224595);
    addRGBColor(n++, 0.985301, 0.635330, 0.221265);
    addRGBColor(n++, 0.986345, 0.640969, 0.217948);
    addRGBColor(n++, 0.987332, 0.646633, 0.214648);
    addRGBColor(n++, 0.988260, 0.652325, 0.211364);
    addRGBColor(n++, 0.989128, 0.658043, 0.208100);
    addRGBColor(n++, 0.989935, 0.663787, 0.204859);
    addRGBColor(n++, 0.990681, 0.669558, 0.201642);
    addRGBColor(n++, 0.991365, 0.675355, 0.198453);
    addRGBColor(n++, 0.991985, 0.681179, 0.195295);
    addRGBColor(n++, 0.992541, 0.687030, 0.192170);
    addRGBColor(n++, 0.993032, 0.692907, 0.189084);
    addRGBColor(n++, 0.993456, 0.698810, 0.186041);
    addRGBColor(n++, 0.993814, 0.704741, 0.183043);
    addRGBColor(n++, 0.994103, 0.710698, 0.180097);
    addRGBColor(n++, 0.994324, 0.716681, 0.177208);
    addRGBColor(n++, 0.994474, 0.722691, 0.174381);
    addRGBColor(n++, 0.994553, 0.728728, 0.171622);
    addRGBColor(n++, 0.994561, 0.734791, 0.168938);
    addRGBColor(n++, 0.994495, 0.740880, 0.166335);
    addRGBColor(n++, 0.994355, 0.746995, 0.163821);
    addRGBColor(n++, 0.994141, 0.753137, 0.161404);
    addRGBColor(n++, 0.993851, 0.759304, 0.159092);
    addRGBColor(n++, 0.993482, 0.765499, 0.156891);
    addRGBColor(n++, 0.993033, 0.771720, 0.154808);
    addRGBColor(n++, 0.992505, 0.777967, 0.152855);
    addRGBColor(n++, 0.991897, 0.784239, 0.151042);
    addRGBColor(n++, 0.991209, 0.790537, 0.149377);
    addRGBColor(n++, 0.990439, 0.796859, 0.147870);
    addRGBColor(n++, 0.989587, 0.803205, 0.146529);
    addRGBColor(n++, 0.988648, 0.809579, 0.145357);
    addRGBColor(n++, 0.987621, 0.815978, 0.144363);
    addRGBColor(n++, 0.986509, 0.822401, 0.143557);
    addRGBColor(n++, 0.985314, 0.828846, 0.142945);
    addRGBColor(n++, 0.984031, 0.835315, 0.142528);
    addRGBColor(n++, 0.982653, 0.841812, 0.142303);
    addRGBColor(n++, 0.981190, 0.848329, 0.142279);
    addRGBColor(n++, 0.979644, 0.854866, 0.142453);
    addRGBColor(n++, 0.977995, 0.861432, 0.142808);
    addRGBColor(n++, 0.976265, 0.868016, 0.143351);
    addRGBColor(n++, 0.974443, 0.874622, 0.144061);
    addRGBColor(n++, 0.972530, 0.881250, 0.144923);
    addRGBColor(n++, 0.970533, 0.887896, 0.145919);
    addRGBColor(n++, 0.968443, 0.894564, 0.147014);
    addRGBColor(n++, 0.966271, 0.901249, 0.148180);
    addRGBColor(n++, 0.964021, 0.907950, 0.149370);
    addRGBColor(n++, 0.961681, 0.914672, 0.150520);
    addRGBColor(n++, 0.959276, 0.921407, 0.151566);
    addRGBColor(n++, 0.956808, 0.928152, 0.152409);
    addRGBColor(n++, 0.954287, 0.934908, 0.152921);
    addRGBColor(n++, 0.951726, 0.941671, 0.152925);
    addRGBColor(n++, 0.949151, 0.948435, 0.152178);
    addRGBColor(n++, 0.946602, 0.955190, 0.150328);
    addRGBColor(n++, 0.944152, 0.961916, 0.146861);
    addRGBColor(n++, 0.941896, 0.968590, 0.140956);
    addRGBColor(n++, 0.940015, 0.975158, 0.131326);
  }
};

class CQChartsPalettePrgn : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePrgn() {
    addDefinedColor(0.0, QColor("#762A83"));
    addDefinedColor(1.0, QColor("#9970AB"));
    addDefinedColor(2.0, QColor("#C2A5CF"));
    addDefinedColor(3.0, QColor("#E7D4E8"));
    addDefinedColor(4.0, QColor("#D9F0D3"));
    addDefinedColor(5.0, QColor("#A6DBA0"));
    addDefinedColor(6.0, QColor("#5AAE61"));
    addDefinedColor(7.0, QColor("#1B7837"));
  }
};

class CQChartsPalettePubugn : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePubugn() {
    addDefinedColor(0.0, QColor("#FFF7FB"));
    addDefinedColor(1.0, QColor("#ECE7F0"));
    addDefinedColor(2.0, QColor("#D0D1E6"));
    addDefinedColor(3.0, QColor("#A6BDDB"));
    addDefinedColor(4.0, QColor("#67A9CF"));
    addDefinedColor(5.0, QColor("#3690C0"));
    addDefinedColor(6.0, QColor("#02818A"));
    addDefinedColor(7.0, QColor("#016540"));
  }
};

class CQChartsPalettePubu : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePubu() {
    addDefinedColor(0.0, QColor("#FFF7FB"));
    addDefinedColor(1.0, QColor("#ECE7F2"));
    addDefinedColor(2.0, QColor("#D0D1E6"));
    addDefinedColor(3.0, QColor("#A6BDDB"));
    addDefinedColor(4.0, QColor("#74A9CF"));
    addDefinedColor(5.0, QColor("#3690C0"));
    addDefinedColor(6.0, QColor("#0570B0"));
    addDefinedColor(7.0, QColor("#034E7B"));
  }
};

class CQChartsPalettePuor : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePuor() {
    addDefinedColor(0.0, QColor("#B35806"));
    addDefinedColor(1.0, QColor("#E08214"));
    addDefinedColor(2.0, QColor("#FDB863"));
    addDefinedColor(3.0, QColor("#FEE0B6"));
    addDefinedColor(4.0, QColor("#D8DAEB"));
    addDefinedColor(5.0, QColor("#B2ABD2"));
    addDefinedColor(6.0, QColor("#8073AC"));
    addDefinedColor(7.0, QColor("#542788"));
  }
};

class CQChartsPalettePurd : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePurd() {
    addDefinedColor(0.0, QColor("#F7F4F9"));
    addDefinedColor(1.0, QColor("#E7E1EF"));
    addDefinedColor(2.0, QColor("#D4B9DA"));
    addDefinedColor(3.0, QColor("#C994C7"));
    addDefinedColor(4.0, QColor("#DF65B0"));
    addDefinedColor(5.0, QColor("#E7298A"));
    addDefinedColor(6.0, QColor("#CE1256"));
    addDefinedColor(7.0, QColor("#91003F"));
  }
};

class CQChartsPalettePurples : public CQChartsDefinedPalette {
 public:
  CQChartsPalettePurples() {
    addDefinedColor(0.0, QColor("#FCFBFD"));
    addDefinedColor(1.0, QColor("#EFEDF5"));
    addDefinedColor(2.0, QColor("#DADAEB"));
    addDefinedColor(3.0, QColor("#BCBDDC"));
    addDefinedColor(4.0, QColor("#9E9AC8"));
    addDefinedColor(5.0, QColor("#807DBA"));
    addDefinedColor(6.0, QColor("#6A51A3"));
    addDefinedColor(7.0, QColor("#4A1486"));
  }
};

class CQChartsPaletteRdbu : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteRdbu() {
    addDefinedColor(0.0, QColor("#B2182B"));
    addDefinedColor(1.0, QColor("#D6604D"));
    addDefinedColor(2.0, QColor("#F4A582"));
    addDefinedColor(3.0, QColor("#FDDBC7"));
    addDefinedColor(4.0, QColor("#D1E5F0"));
    addDefinedColor(5.0, QColor("#92C5DE"));
    addDefinedColor(6.0, QColor("#4393C3"));
    addDefinedColor(7.0, QColor("#2166AC"));
  }
};

class CQChartsPaletteRdgy : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteRdgy() {
    addDefinedColor(0.0, QColor("#B2182B"));
    addDefinedColor(1.0, QColor("#D6604D"));
    addDefinedColor(2.0, QColor("#F4A582"));
    addDefinedColor(3.0, QColor("#FDDBC7"));
    addDefinedColor(4.0, QColor("#E0E0E0"));
    addDefinedColor(5.0, QColor("#BABABA"));
    addDefinedColor(6.0, QColor("#878787"));
    addDefinedColor(7.0, QColor("#4D4D4D"));
  }
};

class CQChartsPaletteRdpu : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteRdpu() {
    addDefinedColor(0.0, QColor("#FFF7F3"));
    addDefinedColor(1.0, QColor("#FDE0DD"));
    addDefinedColor(2.0, QColor("#FCC5C0"));
    addDefinedColor(3.0, QColor("#FA9FB5"));
    addDefinedColor(4.0, QColor("#F768A1"));
    addDefinedColor(5.0, QColor("#DD3497"));
    addDefinedColor(6.0, QColor("#AE017E"));
    addDefinedColor(7.0, QColor("#7A0177"));
  }
};

class CQChartsPaletteRdylbu : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteRdylbu() {
    addDefinedColor(0.0, QColor("#D73027"));
    addDefinedColor(1.0, QColor("#F46D43"));
    addDefinedColor(2.0, QColor("#FDAE61"));
    addDefinedColor(3.0, QColor("#FEE090"));
    addDefinedColor(4.0, QColor("#E0F3F8"));
    addDefinedColor(5.0, QColor("#ABD9E9"));
    addDefinedColor(6.0, QColor("#74ADD1"));
    addDefinedColor(7.0, QColor("#4575B4"));
  }
};

class CQChartsPaletteRdylgn : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteRdylgn() {
    addDefinedColor(0.0, QColor("#D73027"));
    addDefinedColor(1.0, QColor("#F46D43"));
    addDefinedColor(2.0, QColor("#FDAE61"));
    addDefinedColor(3.0, QColor("#FEE08B"));
    addDefinedColor(4.0, QColor("#D9EF8B"));
    addDefinedColor(5.0, QColor("#A6D96A"));
    addDefinedColor(6.0, QColor("#66BD63"));
    addDefinedColor(7.0, QColor("#1A9850"));
  }
};

class CQChartsPaletteReds : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteReds() {
    addDefinedColor(0.0, QColor("#FFF5F0"));
    addDefinedColor(1.0, QColor("#FEE0D2"));
    addDefinedColor(2.0, QColor("#FCBBA1"));
    addDefinedColor(3.0, QColor("#FC9272"));
    addDefinedColor(4.0, QColor("#FB6A4A"));
    addDefinedColor(5.0, QColor("#EF3B2C"));
    addDefinedColor(6.0, QColor("#CB181D"));
    addDefinedColor(7.0, QColor("#99000D"));
  }
};

class CQChartsPaletteSand : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteSand() {
    addDefinedColor(0.0, QColor("#604860"));
    addDefinedColor(1.0, QColor("#784860"));
    addDefinedColor(2.0, QColor("#a86060"));
    addDefinedColor(3.0, QColor("#c07860"));
    addDefinedColor(4.0, QColor("#f0a848"));
    addDefinedColor(5.0, QColor("#f8ca8c"));
    addDefinedColor(6.0, QColor("#feecae"));
    addDefinedColor(7.0, QColor("#fff4c2"));
    addDefinedColor(8.0, QColor("#fff7db"));
    addDefinedColor(9.0, QColor("#fffcf6"));
  }
};

class CQChartsPaletteSet1 : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteSet1() {
    addDefinedColor(0.0, QColor("#E41A1C"));
    addDefinedColor(1.0, QColor("#377EB8"));
    addDefinedColor(2.0, QColor("#4DAF4A"));
    addDefinedColor(3.0, QColor("#984EA3"));
    addDefinedColor(4.0, QColor("#FF7F00"));
    addDefinedColor(5.0, QColor("#FFFF33"));
    addDefinedColor(6.0, QColor("#A65628"));
    addDefinedColor(7.0, QColor("#F781BF"));
  }
};

class CQChartsPaletteSet2 : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteSet2() {
    addDefinedColor(0.0, QColor("#66C2A5"));
    addDefinedColor(1.0, QColor("#FC8D62"));
    addDefinedColor(2.0, QColor("#8DA0CB"));
    addDefinedColor(3.0, QColor("#E78AC3"));
    addDefinedColor(4.0, QColor("#A6D854"));
    addDefinedColor(5.0, QColor("#FFD92F"));
    addDefinedColor(6.0, QColor("#E5C494"));
    addDefinedColor(7.0, QColor("#B3B3B3"));
  }
};

class CQChartsPaletteSet3 : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteSet3() {
    addDefinedColor(0.0, QColor("#8DD3C7"));
    addDefinedColor(1.0, QColor("#FFFFB3"));
    addDefinedColor(2.0, QColor("#BEBADA"));
    addDefinedColor(3.0, QColor("#FB8072"));
    addDefinedColor(4.0, QColor("#80B1D3"));
    addDefinedColor(5.0, QColor("#FDB462"));
    addDefinedColor(6.0, QColor("#B3DE69"));
    addDefinedColor(7.0, QColor("#FCCDE5"));
  }
};

class CQChartsPaletteSpectral : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteSpectral() {
    addDefinedColor(0.0, QColor("#D53E4F"));
    addDefinedColor(1.0, QColor("#F46D43"));
    addDefinedColor(2.0, QColor("#FDAE61"));
    addDefinedColor(3.0, QColor("#FEE08B"));
    addDefinedColor(4.0, QColor("#E6F598"));
    addDefinedColor(5.0, QColor("#ABDDA4"));
    addDefinedColor(6.0, QColor("#66C2A5"));
    addDefinedColor(7.0, QColor("#3288BD"));
  }
};

class CQChartsPaletteViridis : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteViridis() {
    int n = 0;

    addRGBColor(n++, 0.267004, 0.004874, 0.329415);
    addRGBColor(n++, 0.268510, 0.009605, 0.335427);
    addRGBColor(n++, 0.269944, 0.014625, 0.341379);
    addRGBColor(n++, 0.271305, 0.019942, 0.347269);
    addRGBColor(n++, 0.272594, 0.025563, 0.353093);
    addRGBColor(n++, 0.273809, 0.031497, 0.358853);
    addRGBColor(n++, 0.274952, 0.037752, 0.364543);
    addRGBColor(n++, 0.276022, 0.044167, 0.370164);
    addRGBColor(n++, 0.277018, 0.050344, 0.375715);
    addRGBColor(n++, 0.277941, 0.056324, 0.381191);
    addRGBColor(n++, 0.278791, 0.062145, 0.386592);
    addRGBColor(n++, 0.279566, 0.067836, 0.391917);
    addRGBColor(n++, 0.280267, 0.073417, 0.397163);
    addRGBColor(n++, 0.280894, 0.078907, 0.402329);
    addRGBColor(n++, 0.281446, 0.084320, 0.407414);
    addRGBColor(n++, 0.281924, 0.089666, 0.412415);
    addRGBColor(n++, 0.282327, 0.094955, 0.417331);
    addRGBColor(n++, 0.282656, 0.100196, 0.422160);
    addRGBColor(n++, 0.282910, 0.105393, 0.426902);
    addRGBColor(n++, 0.283091, 0.110553, 0.431554);
    addRGBColor(n++, 0.283197, 0.115680, 0.436115);
    addRGBColor(n++, 0.283229, 0.120777, 0.440584);
    addRGBColor(n++, 0.283187, 0.125848, 0.444960);
    addRGBColor(n++, 0.283072, 0.130895, 0.449241);
    addRGBColor(n++, 0.282884, 0.135920, 0.453427);
    addRGBColor(n++, 0.282623, 0.140926, 0.457517);
    addRGBColor(n++, 0.282290, 0.145912, 0.461510);
    addRGBColor(n++, 0.281887, 0.150881, 0.465405);
    addRGBColor(n++, 0.281412, 0.155834, 0.469201);
    addRGBColor(n++, 0.280868, 0.160771, 0.472899);
    addRGBColor(n++, 0.280255, 0.165693, 0.476498);
    addRGBColor(n++, 0.279574, 0.170599, 0.479997);
    addRGBColor(n++, 0.278826, 0.175490, 0.483397);
    addRGBColor(n++, 0.278012, 0.180367, 0.486697);
    addRGBColor(n++, 0.277134, 0.185228, 0.489898);
    addRGBColor(n++, 0.276194, 0.190074, 0.493001);
    addRGBColor(n++, 0.275191, 0.194905, 0.496005);
    addRGBColor(n++, 0.274128, 0.199721, 0.498911);
    addRGBColor(n++, 0.273006, 0.204520, 0.501721);
    addRGBColor(n++, 0.271828, 0.209303, 0.504434);
    addRGBColor(n++, 0.270595, 0.214069, 0.507052);
    addRGBColor(n++, 0.269308, 0.218818, 0.509577);
    addRGBColor(n++, 0.267968, 0.223549, 0.512008);
    addRGBColor(n++, 0.266580, 0.228262, 0.514349);
    addRGBColor(n++, 0.265145, 0.232956, 0.516599);
    addRGBColor(n++, 0.263663, 0.237631, 0.518762);
    addRGBColor(n++, 0.262138, 0.242286, 0.520837);
    addRGBColor(n++, 0.260571, 0.246922, 0.522828);
    addRGBColor(n++, 0.258965, 0.251537, 0.524736);
    addRGBColor(n++, 0.257322, 0.256130, 0.526563);
    addRGBColor(n++, 0.255645, 0.260703, 0.528312);
    addRGBColor(n++, 0.253935, 0.265254, 0.529983);
    addRGBColor(n++, 0.252194, 0.269783, 0.531579);
    addRGBColor(n++, 0.250425, 0.274290, 0.533103);
    addRGBColor(n++, 0.248629, 0.278775, 0.534556);
    addRGBColor(n++, 0.246811, 0.283237, 0.535941);
    addRGBColor(n++, 0.244972, 0.287675, 0.537260);
    addRGBColor(n++, 0.243113, 0.292092, 0.538516);
    addRGBColor(n++, 0.241237, 0.296485, 0.539709);
    addRGBColor(n++, 0.239346, 0.300855, 0.540844);
    addRGBColor(n++, 0.237441, 0.305202, 0.541921);
    addRGBColor(n++, 0.235526, 0.309527, 0.542944);
    addRGBColor(n++, 0.233603, 0.313828, 0.543914);
    addRGBColor(n++, 0.231674, 0.318106, 0.544834);
    addRGBColor(n++, 0.229739, 0.322361, 0.545706);
    addRGBColor(n++, 0.227802, 0.326594, 0.546532);
    addRGBColor(n++, 0.225863, 0.330805, 0.547314);
    addRGBColor(n++, 0.223925, 0.334994, 0.548053);
    addRGBColor(n++, 0.221989, 0.339161, 0.548752);
    addRGBColor(n++, 0.220057, 0.343307, 0.549413);
    addRGBColor(n++, 0.218130, 0.347432, 0.550038);
    addRGBColor(n++, 0.216210, 0.351535, 0.550627);
    addRGBColor(n++, 0.214298, 0.355619, 0.551184);
    addRGBColor(n++, 0.212395, 0.359683, 0.551710);
    addRGBColor(n++, 0.210503, 0.363727, 0.552206);
    addRGBColor(n++, 0.208623, 0.367752, 0.552675);
    addRGBColor(n++, 0.206756, 0.371758, 0.553117);
    addRGBColor(n++, 0.204903, 0.375746, 0.553533);
    addRGBColor(n++, 0.203063, 0.379716, 0.553925);
    addRGBColor(n++, 0.201239, 0.383670, 0.554294);
    addRGBColor(n++, 0.199430, 0.387607, 0.554642);
    addRGBColor(n++, 0.197636, 0.391528, 0.554969);
    addRGBColor(n++, 0.195860, 0.395433, 0.555276);
    addRGBColor(n++, 0.194100, 0.399323, 0.555565);
    addRGBColor(n++, 0.192357, 0.403199, 0.555836);
    addRGBColor(n++, 0.190631, 0.407061, 0.556089);
    addRGBColor(n++, 0.188923, 0.410910, 0.556326);
    addRGBColor(n++, 0.187231, 0.414746, 0.556547);
    addRGBColor(n++, 0.185556, 0.418570, 0.556753);
    addRGBColor(n++, 0.183898, 0.422383, 0.556944);
    addRGBColor(n++, 0.182256, 0.426184, 0.557120);
    addRGBColor(n++, 0.180629, 0.429975, 0.557282);
    addRGBColor(n++, 0.179019, 0.433756, 0.557430);
    addRGBColor(n++, 0.177423, 0.437527, 0.557565);
    addRGBColor(n++, 0.175841, 0.441290, 0.557685);
    addRGBColor(n++, 0.174274, 0.445044, 0.557792);
    addRGBColor(n++, 0.172719, 0.448791, 0.557885);
    addRGBColor(n++, 0.171176, 0.452530, 0.557965);
    addRGBColor(n++, 0.169646, 0.456262, 0.558030);
    addRGBColor(n++, 0.168126, 0.459988, 0.558082);
    addRGBColor(n++, 0.166617, 0.463708, 0.558119);
    addRGBColor(n++, 0.165117, 0.467423, 0.558141);
    addRGBColor(n++, 0.163625, 0.471133, 0.558148);
    addRGBColor(n++, 0.162142, 0.474838, 0.558140);
    addRGBColor(n++, 0.160665, 0.478540, 0.558115);
    addRGBColor(n++, 0.159194, 0.482237, 0.558073);
    addRGBColor(n++, 0.157729, 0.485932, 0.558013);
    addRGBColor(n++, 0.156270, 0.489624, 0.557936);
    addRGBColor(n++, 0.154815, 0.493313, 0.557840);
    addRGBColor(n++, 0.153364, 0.497000, 0.557724);
    addRGBColor(n++, 0.151918, 0.500685, 0.557587);
    addRGBColor(n++, 0.150476, 0.504369, 0.557430);
    addRGBColor(n++, 0.149039, 0.508051, 0.557250);
    addRGBColor(n++, 0.147607, 0.511733, 0.557049);
    addRGBColor(n++, 0.146180, 0.515413, 0.556823);
    addRGBColor(n++, 0.144759, 0.519093, 0.556572);
    addRGBColor(n++, 0.143343, 0.522773, 0.556295);
    addRGBColor(n++, 0.141935, 0.526453, 0.555991);
    addRGBColor(n++, 0.140536, 0.530132, 0.555659);
    addRGBColor(n++, 0.139147, 0.533812, 0.555298);
    addRGBColor(n++, 0.137770, 0.537492, 0.554906);
    addRGBColor(n++, 0.136408, 0.541173, 0.554483);
    addRGBColor(n++, 0.135066, 0.544853, 0.554029);
    addRGBColor(n++, 0.133743, 0.548535, 0.553541);
    addRGBColor(n++, 0.132444, 0.552216, 0.553018);
    addRGBColor(n++, 0.131172, 0.555899, 0.552459);
    addRGBColor(n++, 0.129933, 0.559582, 0.551864);
    addRGBColor(n++, 0.128729, 0.563265, 0.551229);
    addRGBColor(n++, 0.127568, 0.566949, 0.550556);
    addRGBColor(n++, 0.126453, 0.570633, 0.549841);
    addRGBColor(n++, 0.125394, 0.574318, 0.549086);
    addRGBColor(n++, 0.124395, 0.578002, 0.548287);
    addRGBColor(n++, 0.123463, 0.581687, 0.547445);
    addRGBColor(n++, 0.122606, 0.585371, 0.546557);
    addRGBColor(n++, 0.121831, 0.589055, 0.545623);
    addRGBColor(n++, 0.121148, 0.592739, 0.544641);
    addRGBColor(n++, 0.120565, 0.596422, 0.543611);
    addRGBColor(n++, 0.120092, 0.600104, 0.542530);
    addRGBColor(n++, 0.119738, 0.603785, 0.541400);
    addRGBColor(n++, 0.119512, 0.607464, 0.540218);
    addRGBColor(n++, 0.119423, 0.611141, 0.538982);
    addRGBColor(n++, 0.119483, 0.614817, 0.537692);
    addRGBColor(n++, 0.119699, 0.618490, 0.536347);
    addRGBColor(n++, 0.120081, 0.622161, 0.534946);
    addRGBColor(n++, 0.120638, 0.625828, 0.533488);
    addRGBColor(n++, 0.121380, 0.629492, 0.531973);
    addRGBColor(n++, 0.122312, 0.633153, 0.530398);
    addRGBColor(n++, 0.123444, 0.636809, 0.528763);
    addRGBColor(n++, 0.124780, 0.640461, 0.527068);
    addRGBColor(n++, 0.126326, 0.644107, 0.525311);
    addRGBColor(n++, 0.128087, 0.647749, 0.523491);
    addRGBColor(n++, 0.130067, 0.651384, 0.521608);
    addRGBColor(n++, 0.132268, 0.655014, 0.519661);
    addRGBColor(n++, 0.134692, 0.658636, 0.517649);
    addRGBColor(n++, 0.137339, 0.662252, 0.515571);
    addRGBColor(n++, 0.140210, 0.665859, 0.513427);
    addRGBColor(n++, 0.143303, 0.669459, 0.511215);
    addRGBColor(n++, 0.146616, 0.673050, 0.508936);
    addRGBColor(n++, 0.150148, 0.676631, 0.506589);
    addRGBColor(n++, 0.153894, 0.680203, 0.504172);
    addRGBColor(n++, 0.157851, 0.683765, 0.501686);
    addRGBColor(n++, 0.162016, 0.687316, 0.499129);
    addRGBColor(n++, 0.166383, 0.690856, 0.496502);
    addRGBColor(n++, 0.170948, 0.694384, 0.493803);
    addRGBColor(n++, 0.175707, 0.697900, 0.491033);
    addRGBColor(n++, 0.180653, 0.701402, 0.488189);
    addRGBColor(n++, 0.185783, 0.704891, 0.485273);
    addRGBColor(n++, 0.191090, 0.708366, 0.482284);
    addRGBColor(n++, 0.196571, 0.711827, 0.479221);
    addRGBColor(n++, 0.202219, 0.715272, 0.476084);
    addRGBColor(n++, 0.208030, 0.718701, 0.472873);
    addRGBColor(n++, 0.214000, 0.722114, 0.469588);
    addRGBColor(n++, 0.220124, 0.725509, 0.466226);
    addRGBColor(n++, 0.226397, 0.728888, 0.462789);
    addRGBColor(n++, 0.232815, 0.732247, 0.459277);
    addRGBColor(n++, 0.239374, 0.735588, 0.455688);
    addRGBColor(n++, 0.246070, 0.738910, 0.452024);
    addRGBColor(n++, 0.252899, 0.742211, 0.448284);
    addRGBColor(n++, 0.259857, 0.745492, 0.444467);
    addRGBColor(n++, 0.266941, 0.748751, 0.440573);
    addRGBColor(n++, 0.274149, 0.751988, 0.436601);
    addRGBColor(n++, 0.281477, 0.755203, 0.432552);
    addRGBColor(n++, 0.288921, 0.758394, 0.428426);
    addRGBColor(n++, 0.296479, 0.761561, 0.424223);
    addRGBColor(n++, 0.304148, 0.764704, 0.419943);
    addRGBColor(n++, 0.311925, 0.767822, 0.415586);
    addRGBColor(n++, 0.319809, 0.770914, 0.411152);
    addRGBColor(n++, 0.327796, 0.773980, 0.406640);
    addRGBColor(n++, 0.335885, 0.777018, 0.402049);
    addRGBColor(n++, 0.344074, 0.780029, 0.397381);
    addRGBColor(n++, 0.352360, 0.783011, 0.392636);
    addRGBColor(n++, 0.360741, 0.785964, 0.387814);
    addRGBColor(n++, 0.369214, 0.788888, 0.382914);
    addRGBColor(n++, 0.377779, 0.791781, 0.377939);
    addRGBColor(n++, 0.386433, 0.794644, 0.372886);
    addRGBColor(n++, 0.395174, 0.797475, 0.367757);
    addRGBColor(n++, 0.404001, 0.800275, 0.362552);
    addRGBColor(n++, 0.412913, 0.803041, 0.357269);
    addRGBColor(n++, 0.421908, 0.805774, 0.351910);
    addRGBColor(n++, 0.430983, 0.808473, 0.346476);
    addRGBColor(n++, 0.440137, 0.811138, 0.340967);
    addRGBColor(n++, 0.449368, 0.813768, 0.335384);
    addRGBColor(n++, 0.458674, 0.816363, 0.329727);
    addRGBColor(n++, 0.468053, 0.818921, 0.323998);
    addRGBColor(n++, 0.477504, 0.821444, 0.318195);
    addRGBColor(n++, 0.487026, 0.823929, 0.312321);
    addRGBColor(n++, 0.496615, 0.826376, 0.306377);
    addRGBColor(n++, 0.506271, 0.828786, 0.300362);
    addRGBColor(n++, 0.515992, 0.831158, 0.294279);
    addRGBColor(n++, 0.525776, 0.833491, 0.288127);
    addRGBColor(n++, 0.535621, 0.835785, 0.281908);
    addRGBColor(n++, 0.545524, 0.838039, 0.275626);
    addRGBColor(n++, 0.555484, 0.840254, 0.269281);
    addRGBColor(n++, 0.565498, 0.842430, 0.262877);
    addRGBColor(n++, 0.575563, 0.844566, 0.256415);
    addRGBColor(n++, 0.585678, 0.846661, 0.249897);
    addRGBColor(n++, 0.595839, 0.848717, 0.243329);
    addRGBColor(n++, 0.606045, 0.850733, 0.236712);
    addRGBColor(n++, 0.616293, 0.852709, 0.230052);
    addRGBColor(n++, 0.626579, 0.854645, 0.223353);
    addRGBColor(n++, 0.636902, 0.856542, 0.216620);
    addRGBColor(n++, 0.647257, 0.858400, 0.209861);
    addRGBColor(n++, 0.657642, 0.860219, 0.203082);
    addRGBColor(n++, 0.668054, 0.861999, 0.196293);
    addRGBColor(n++, 0.678489, 0.863742, 0.189503);
    addRGBColor(n++, 0.688944, 0.865448, 0.182725);
    addRGBColor(n++, 0.699415, 0.867117, 0.175971);
    addRGBColor(n++, 0.709898, 0.868751, 0.169257);
    addRGBColor(n++, 0.720391, 0.870350, 0.162603);
    addRGBColor(n++, 0.730889, 0.871916, 0.156029);
    addRGBColor(n++, 0.741388, 0.873449, 0.149561);
    addRGBColor(n++, 0.751884, 0.874951, 0.143228);
    addRGBColor(n++, 0.762373, 0.876424, 0.137064);
    addRGBColor(n++, 0.772852, 0.877868, 0.131109);
    addRGBColor(n++, 0.783315, 0.879285, 0.125405);
    addRGBColor(n++, 0.793760, 0.880678, 0.120005);
    addRGBColor(n++, 0.804182, 0.882046, 0.114965);
    addRGBColor(n++, 0.814576, 0.883393, 0.110347);
    addRGBColor(n++, 0.824940, 0.884720, 0.106217);
    addRGBColor(n++, 0.835270, 0.886029, 0.102646);
    addRGBColor(n++, 0.845561, 0.887322, 0.099702);
    addRGBColor(n++, 0.855810, 0.888601, 0.097452);
    addRGBColor(n++, 0.866013, 0.889868, 0.095953);
    addRGBColor(n++, 0.876168, 0.891125, 0.095250);
    addRGBColor(n++, 0.886271, 0.892374, 0.095374);
    addRGBColor(n++, 0.896320, 0.893616, 0.096335);
    addRGBColor(n++, 0.906311, 0.894855, 0.098125);
    addRGBColor(n++, 0.916242, 0.896091, 0.100717);
    addRGBColor(n++, 0.926106, 0.897330, 0.104071);
    addRGBColor(n++, 0.935904, 0.898570, 0.108131);
    addRGBColor(n++, 0.945636, 0.899815, 0.112838);
    addRGBColor(n++, 0.955300, 0.901065, 0.118128);
    addRGBColor(n++, 0.964894, 0.902323, 0.123941);
    addRGBColor(n++, 0.974417, 0.903590, 0.130215);
    addRGBColor(n++, 0.983868, 0.904867, 0.136897);
    addRGBColor(n++, 0.993248, 0.906157, 0.143936);
  }
};

class CQChartsPaletteWhylrd : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteWhylrd() {
    addDefinedColor(0.0, QColor("#ffffff"));
    addDefinedColor(1.0, QColor("#ffee00"));
    addDefinedColor(2.0, QColor("#ff7000"));
    addDefinedColor(3.0, QColor("#ee0000"));
    addDefinedColor(4.0, QColor("#7f0000"));
  }
};

class CQChartsPaletteYlgnbu : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteYlgnbu() {
    addDefinedColor(0.0, QColor("#FFFFD9"));
    addDefinedColor(1.0, QColor("#EDF8B1"));
    addDefinedColor(2.0, QColor("#C7E9B4"));
    addDefinedColor(3.0, QColor("#7FCDBB"));
    addDefinedColor(4.0, QColor("#41B6C4"));
    addDefinedColor(5.0, QColor("#1D91C0"));
    addDefinedColor(6.0, QColor("#225EA8"));
    addDefinedColor(7.0, QColor("#0C2C84"));
  }
};

class CQChartsPaletteYlgn : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteYlgn() {
    addDefinedColor(0.0, QColor("#FFFFE5"));
    addDefinedColor(1.0, QColor("#F7FCB9"));
    addDefinedColor(2.0, QColor("#D9F0A3"));
    addDefinedColor(3.0, QColor("#ADDD8E"));
    addDefinedColor(4.0, QColor("#78C679"));
    addDefinedColor(5.0, QColor("#41AB5D"));
    addDefinedColor(6.0, QColor("#238443"));
    addDefinedColor(7.0, QColor("#005A32"));
  }
};

class CQChartsPaletteYlorbr : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteYlorbr() {
    addDefinedColor(0.0, QColor("#FFFFE5"));
    addDefinedColor(1.0, QColor("#FFF7BC"));
    addDefinedColor(2.0, QColor("#FEE391"));
    addDefinedColor(3.0, QColor("#FEC44F"));
    addDefinedColor(4.0, QColor("#FE9929"));
    addDefinedColor(5.0, QColor("#EC7014"));
    addDefinedColor(6.0, QColor("#CC4C02"));
    addDefinedColor(7.0, QColor("#8C2D04"));
  }
};

class CQChartsPaletteYlorrd : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteYlorrd() {
    addDefinedColor(0.0, QColor("#FFFFCC"));
    addDefinedColor(1.0, QColor("#FFEDA0"));
    addDefinedColor(2.0, QColor("#FED976"));
    addDefinedColor(3.0, QColor("#FEB24C"));
    addDefinedColor(4.0, QColor("#FD8D3C"));
    addDefinedColor(5.0, QColor("#FC4E2A"));
    addDefinedColor(6.0, QColor("#E31A1C"));
    addDefinedColor(7.0, QColor("#B10026"));
  }
};

class CQChartsPaletteYlrd : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteYlrd() {
    addDefinedColor(0.0, QColor("#ffee00"));
    addDefinedColor(1.0, QColor("#ff7000"));
    addDefinedColor(2.0, QColor("#ee0000"));
    addDefinedColor(3.0, QColor("#7f0000"));
  }
};

class CQChartsPaletteDistinct1 : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteDistinct1() {
    int n = 0;

    addDefinedColor(n++, QColor("#e6194b"));
    addDefinedColor(n++, QColor("#3cb44b"));
    addDefinedColor(n++, QColor("#ffe119"));
    addDefinedColor(n++, QColor("#4363d8"));
    addDefinedColor(n++, QColor("#f58231"));
    addDefinedColor(n++, QColor("#911eb4"));
    addDefinedColor(n++, QColor("#46f0f0"));
    addDefinedColor(n++, QColor("#f032e6"));
    addDefinedColor(n++, QColor("#bcf60c"));
    addDefinedColor(n++, QColor("#fabebe"));
    addDefinedColor(n++, QColor("#008080"));
    addDefinedColor(n++, QColor("#e6beff"));
    addDefinedColor(n++, QColor("#9a6324"));
    addDefinedColor(n++, QColor("#fffac8"));
    addDefinedColor(n++, QColor("#800000"));
    addDefinedColor(n++, QColor("#aaffc3"));
    addDefinedColor(n++, QColor("#808000"));
    addDefinedColor(n++, QColor("#ffd8b1"));
    addDefinedColor(n++, QColor("#000075"));
    addDefinedColor(n++, QColor("#808080"));
//  addDefinedColor(n++, QColor("#ffffff"));
//  addDefinedColor(n++, QColor("#000000"));

    setDistinct(true);
  }
};

class CQChartsPaletteDistinct2 : public CQChartsDefinedPalette {
 public:
  CQChartsPaletteDistinct2() {
    int n = 0;

    addDefinedColor(n++, QColor("#000000"));
    addDefinedColor(n++, QColor("#00FF00"));
    addDefinedColor(n++, QColor("#0000FF"));
    addDefinedColor(n++, QColor("#FF0000"));
    addDefinedColor(n++, QColor("#01FFFE"));
    addDefinedColor(n++, QColor("#FFA6FE"));
    addDefinedColor(n++, QColor("#FFDB66"));
    addDefinedColor(n++, QColor("#006401"));
    addDefinedColor(n++, QColor("#010067"));
    addDefinedColor(n++, QColor("#95003A"));
    addDefinedColor(n++, QColor("#007DB5"));
    addDefinedColor(n++, QColor("#FF00F6"));
    addDefinedColor(n++, QColor("#FFEEE8"));
    addDefinedColor(n++, QColor("#774D00"));
    addDefinedColor(n++, QColor("#90FB92"));
    addDefinedColor(n++, QColor("#0076FF"));
    addDefinedColor(n++, QColor("#D5FF00"));
    addDefinedColor(n++, QColor("#FF937E"));
    addDefinedColor(n++, QColor("#6A826C"));
    addDefinedColor(n++, QColor("#FF029D"));
    addDefinedColor(n++, QColor("#FE8900"));
    addDefinedColor(n++, QColor("#7A4782"));
    addDefinedColor(n++, QColor("#7E2DD2"));
    addDefinedColor(n++, QColor("#85A900"));
    addDefinedColor(n++, QColor("#FF0056"));
    addDefinedColor(n++, QColor("#A42400"));
    addDefinedColor(n++, QColor("#00AE7E"));
    addDefinedColor(n++, QColor("#683D3B"));
    addDefinedColor(n++, QColor("#BDC6FF"));
    addDefinedColor(n++, QColor("#263400"));
    addDefinedColor(n++, QColor("#BDD393"));
    addDefinedColor(n++, QColor("#00B917"));
    addDefinedColor(n++, QColor("#9E008E"));
    addDefinedColor(n++, QColor("#001544"));
    addDefinedColor(n++, QColor("#C28C9F"));
    addDefinedColor(n++, QColor("#FF74A3"));
    addDefinedColor(n++, QColor("#01D0FF"));
    addDefinedColor(n++, QColor("#004754"));
    addDefinedColor(n++, QColor("#E56FFE"));
    addDefinedColor(n++, QColor("#788231"));
    addDefinedColor(n++, QColor("#0E4CA1"));
    addDefinedColor(n++, QColor("#91D0CB"));
    addDefinedColor(n++, QColor("#BE9970"));
    addDefinedColor(n++, QColor("#968AE8"));
    addDefinedColor(n++, QColor("#BB8800"));
    addDefinedColor(n++, QColor("#43002C"));
    addDefinedColor(n++, QColor("#DEFF74"));
    addDefinedColor(n++, QColor("#00FFC6"));
    addDefinedColor(n++, QColor("#FFE502"));
    addDefinedColor(n++, QColor("#620E00"));
    addDefinedColor(n++, QColor("#008F9C"));
    addDefinedColor(n++, QColor("#98FF52"));
    addDefinedColor(n++, QColor("#7544B1"));
    addDefinedColor(n++, QColor("#B500FF"));
    addDefinedColor(n++, QColor("#00FF78"));
    addDefinedColor(n++, QColor("#FF6E41"));
    addDefinedColor(n++, QColor("#005F39"));
    addDefinedColor(n++, QColor("#6B6882"));
    addDefinedColor(n++, QColor("#5FAD4E"));
    addDefinedColor(n++, QColor("#A75740"));
    addDefinedColor(n++, QColor("#A5FFD2"));
    addDefinedColor(n++, QColor("#FFB167"));
    addDefinedColor(n++, QColor("#009BFF"));
    addDefinedColor(n++, QColor("#E85EBE"));

    setDistinct(true);
  }
};

//------

CQChartsThemeMgr *
CQChartsThemeMgr::
instance()
{
  static CQChartsThemeMgr *inst;

  if (! inst) {
    inst = new CQChartsThemeMgr;

    inst->init();
  }

  return inst;
}

CQChartsThemeMgr::
CQChartsThemeMgr()
{
}

CQChartsThemeMgr::
~CQChartsThemeMgr()
{
  for (auto &namedPalette : namedPalettes_) {
    PaletteData &paletteData = namedPalette.second;

    delete paletteData.original;
    delete paletteData.current;
  }

  for (auto &nameTheme : themes_)
    delete nameTheme.second;
}

void
CQChartsThemeMgr::
init()
{
  addNamedPalette("default" , new CQChartsDefaultPalette );
  addNamedPalette("palette1", new CQChartsPalette1       );
  addNamedPalette("palette2", new CQChartsPalette2       );

  // multi-color
  addNamedPalette("accent"  , new CQChartsPaletteAccent  );
  addNamedPalette("brbg"    , new CQChartsPaletteBrbg    );
  addNamedPalette("dark2"   , new CQChartsPaletteDark2   );
  addNamedPalette("inferno" , new CQChartsPaletteInferno );
  addNamedPalette("jet"     , new CQChartsPaletteJet     );
  addNamedPalette("magma"   , new CQChartsPaletteMagma   );
  addNamedPalette("paired"  , new CQChartsPalettePaired  );
  addNamedPalette("parula"  , new CQChartsPaletteParula  );
  addNamedPalette("pastel1" , new CQChartsPalettePastel1 );
  addNamedPalette("pastel2" , new CQChartsPalettePastel2 );
  addNamedPalette("plasma"  , new CQChartsPalettePlasma  );
  addNamedPalette("pubugn"  , new CQChartsPalettePubugn  );
  addNamedPalette("rdylbu"  , new CQChartsPaletteRdylbu  );
  addNamedPalette("rdylgn"  , new CQChartsPaletteRdylgn  );
  addNamedPalette("set1"    , new CQChartsPaletteSet1    );
  addNamedPalette("set2"    , new CQChartsPaletteSet2    );
  addNamedPalette("set3"    , new CQChartsPaletteSet3    );
  addNamedPalette("spectral", new CQChartsPaletteSpectral);
  addNamedPalette("viridis" , new CQChartsPaletteViridis );
  addNamedPalette("whylrd"  , new CQChartsPaletteWhylrd  );
  addNamedPalette("ylgnbu"  , new CQChartsPaletteYlgnbu  );

  // two-color
  addNamedPalette("gnpu"    , new CQChartsPaletteGnpu    );
  addNamedPalette("moreland", new CQChartsPaletteMoreland);
  addNamedPalette("piyg"    , new CQChartsPalettePiyg    );
  addNamedPalette("prgn"    , new CQChartsPalettePrgn    );
  addNamedPalette("puor"    , new CQChartsPalettePuor    );
  addNamedPalette("purd"    , new CQChartsPalettePurd    );
  addNamedPalette("rdbu"    , new CQChartsPaletteRdbu    );
  addNamedPalette("rdgy"    , new CQChartsPaletteRdgy    );
  addNamedPalette("rdpu"    , new CQChartsPaletteRdpu    );

  // gradient
  addNamedPalette("reds"    , new CQChartsPaletteReds    );
  addNamedPalette("greens"  , new CQChartsPaletteGreens  );
  addNamedPalette("blues"   , new CQChartsPaletteBlues   );
  addNamedPalette("oranges" , new CQChartsPaletteOranges );
  addNamedPalette("purples" , new CQChartsPalettePurples );
  addNamedPalette("greys"   , new CQChartsPaletteGreys   );
  addNamedPalette("bugn"    , new CQChartsPaletteBugn    );
  addNamedPalette("bupu"    , new CQChartsPaletteBupu    );
  addNamedPalette("chromajs", new CQChartsPaletteChromajs);
  addNamedPalette("gnbu"    , new CQChartsPaletteGnbu    );
  addNamedPalette("orrd"    , new CQChartsPaletteOrrd    );
  addNamedPalette("pubu"    , new CQChartsPalettePubu    );
  addNamedPalette("sand"    , new CQChartsPaletteSand    );
  addNamedPalette("ylgn"    , new CQChartsPaletteYlgn    );
  addNamedPalette("ylorbr"  , new CQChartsPaletteYlorbr  );
  addNamedPalette("ylorrd"  , new CQChartsPaletteYlorrd  );
  addNamedPalette("ylrd"    , new CQChartsPaletteYlrd    );

  // distinct
  addNamedPalette("distinct1", new CQChartsPaletteDistinct1);
  addNamedPalette("distinct2", new CQChartsPaletteDistinct2);

  //---

  addTheme("default" , new CQChartsDefaultTheme);
  addTheme("palette1", new CQChartsTheme1);
  addTheme("palette2", new CQChartsTheme2);
}

void
CQChartsThemeMgr::
addNamedPalette(const QString &name, CQChartsGradientPalette *palette)
{
  auto p = namedPalettes_.find(name);
  assert(p == namedPalettes_.end());

  palette->setName(name);

  PaletteData paletteData;

  paletteData.current  = palette;
  paletteData.original = palette->dup();

  namedPalettes_[name] = paletteData;

  emit palettesChanged();
}

CQChartsGradientPalette *
CQChartsThemeMgr::
getNamedPalette(const QString &name) const
{
  auto p = namedPalettes_.find(name);

  if (p == namedPalettes_.end())
    return nullptr;

  const PaletteData &paletteData = (*p).second;

  return paletteData.current;
}

void
CQChartsThemeMgr::
getPaletteNames(QStringList &names) const
{
  for (const auto &p : namedPalettes_)
    names.push_back(p.first);
}

void
CQChartsThemeMgr::
addTheme(const QString &name, CQChartsTheme *theme)
{
  auto p = themes_.find(name);
  assert(p == themes_.end());

  theme->setName(name);

  themes_[name] = theme;

  connect(theme, SIGNAL(themeChanged()), this, SLOT(themeChangedSlot()));

  emit themesChanged();
}

CQChartsTheme *
CQChartsThemeMgr::
getTheme(const QString &name) const
{
  auto p = themes_.find(name);

  if (p == themes_.end())
    return nullptr;

  return (*p).second;
}

void
CQChartsThemeMgr::
getThemeNames(QStringList &names) const
{
  for (const auto &p : themes_)
    names.push_back(p.first);
}

void
CQChartsThemeMgr::
resetPalette(const QString &name)
{
  auto p = namedPalettes_.find(name);
  assert(p != namedPalettes_.end());

  PaletteData &paletteData = (*p).second;

  *paletteData.current = *paletteData.original;
}

void
CQChartsThemeMgr::
themeChangedSlot()
{
  CQChartsTheme *theme = qobject_cast<CQChartsTheme *>(sender());
  if (! theme) return;

  emit themeChanged(theme->name());
}

//------

CQChartsTheme::
CQChartsTheme()
{
  addNamedPalettes();
}

void
CQChartsTheme::
addNamedPalettes()
{
  // add 10 palettes by default
  addNamedPalette("default"  );
  addNamedPalette("palette1" );
  addNamedPalette("palette2" );
  addNamedPalette("set1"     );
  addNamedPalette("set2"     );
  addNamedPalette("set3"     );
  addNamedPalette("plasma"   );
  addNamedPalette("chromajs" );
  addNamedPalette("distinct1");
  addNamedPalette("distinct2");

#if 0
  addNamedPalette("default" );
  addNamedPalette("palette1");
  addNamedPalette("palette2");

  // multi-color
  addNamedPalette("accent"  );
  addNamedPalette("brbg"    );
  addNamedPalette("dark2"   );
  addNamedPalette("inferno" );
  addNamedPalette("jet"     );
  addNamedPalette("magma"   );
  addNamedPalette("paired"  );
  addNamedPalette("parula"  );
  addNamedPalette("pastel1" );
  addNamedPalette("pastel2" );
  addNamedPalette("plasma"  );
  addNamedPalette("pubugn"  );
  addNamedPalette("rdylbu"  );
  addNamedPalette("rdylgn"  );
  addNamedPalette("set1"    );
  addNamedPalette("set2"    );
  addNamedPalette("set3"    );
  addNamedPalette("spectral");
  addNamedPalette("viridis" );
  addNamedPalette("whylrd"  );
  addNamedPalette("ylgnbu"  );

  // two-color
  addNamedPalette("gnpu"    );
  addNamedPalette("moreland");
  addNamedPalette("piyg"    );
  addNamedPalette("prgn"    );
  addNamedPalette("puor"    );
  addNamedPalette("purd"    );
  addNamedPalette("rdbu"    );
  addNamedPalette("rdgy"    );
  addNamedPalette("rdpu"    );

  // gradient
  addNamedPalette("reds"    );
  addNamedPalette("greens"  );
  addNamedPalette("blues"   );
  addNamedPalette("oranges" );
  addNamedPalette("purples" );
  addNamedPalette("greys"   );
  addNamedPalette("bugn"    );
  addNamedPalette("bupu"    );
  addNamedPalette("chromajs");
  addNamedPalette("gnbu"    );
  addNamedPalette("orrd"    );
  addNamedPalette("pubu"    );
  addNamedPalette("sand"    );
  addNamedPalette("ylgn"    );
  addNamedPalette("ylorbr"  );
  addNamedPalette("ylorrd"  );
  addNamedPalette("ylrd"    );
#endif
}

CQChartsTheme::
~CQChartsTheme()
{
}

CQChartsGradientPalette *
CQChartsTheme::
palette(int i) const
{
  int i1 = i % palettes_.size();

  return palettes_[i1];
}

void
CQChartsTheme::
setPalette(int i, CQChartsGradientPalette *palette)
{
  // validate destination position
  int n = palettes_.size();
  assert(i >= 0 && i < n);

  palettes_[i] = palette;

  emit themeChanged();
}

void
CQChartsTheme::
addNamedPalette(const QString &name)
{
  palettes_.push_back(CQChartsThemeMgrInst->getNamedPalette(name));

  emit themeChanged();
}

void
CQChartsTheme::
removeNamedPalette(const QString &name)
{
  int pos = paletteInd(name);
  if (pos < 0) return;

  int n = palettes_.size();

  for (int i = pos + 1; i < n; ++i)
    palettes_[i - 1] = palettes_[i];

  palettes_.pop_back();

  //---

  emit themeChanged();
}

void
CQChartsTheme::
setNamedPalette(int i, const QString &name)
{
  setPalette(i, CQChartsThemeMgrInst->getNamedPalette(name));
}

void
CQChartsTheme::
movePalette(const QString &name, int pos)
{
  // validate destination position
  int n = palettes_.size();
  assert(pos >= 0 && pos < n);

  //---

  // get palette position
  int pos1 = paletteInd(name);
  assert(pos1 >= 0);

  if (pos1 == pos)
    return;

  //---

  // remove palette to move from list
  CQChartsGradientPalette *palette = palettes_[pos1];

  for (int i = pos1 + 1; i < n; ++i)
    palettes_[i - 1] = palettes_[i];

  palettes_[n - 1] = nullptr;

  //---

  for (int i = n - 1; i > pos; --i)
    palettes_[i] = palettes_[i - 1];

  palettes_[pos] = palette;

  //---

  emit themeChanged();
}

int
CQChartsTheme::
paletteInd(const QString &name) const
{
  int n = palettes_.size();

  for (int i = 0; i < n; ++i)
    if (palettes_[i]->name() == name)
      return i;

  return -1;
}

#if 0
void
CQChartsTheme::
shiftPalettes(int n)
{
  Palettes palettes;

  palettes.resize(n);

  for (int i = 0; i < n; ++i)
    palettes[i] = palettes_[i];

  int n1 = palettes_.size() - n;

  for (int i = 0; i < n1; ++i)
    palettes_[i] = palettes_[i + n];

  for (int i = 0; i < n; ++i)
    palettes_[i + n1] = palettes[i];

  emit themeChanged();
}
#endif

void
CQChartsTheme::
setSelectColor(const QColor &c)
{
  selectColor_ = c;

  emit themeChanged();
}

void
CQChartsTheme::
setInsideColor(const QColor &c)
{
  insideColor_ = c;

  emit themeChanged();
}

//---

CQChartsDefaultTheme::
CQChartsDefaultTheme()
{
  setDesc("Default");

  //---

  movePalette("set2"    , 0);
  movePalette("palette1", 1);
  movePalette("palette2", 2);

  //---

  selectColor_ = QColor("#ee4444");
  insideColor_ = QColor("#44ee44");
}

CQChartsTheme1::
CQChartsTheme1()
{
  setDesc("Theme 1");

  //---

  movePalette("palette1", 0);
  movePalette("palette2", 1);
  movePalette("set2"    , 2);

  //---

  selectColor_ = QColor("#44ee44");
  insideColor_ = QColor("#4444ee");
}

CQChartsTheme2::
CQChartsTheme2()
{
  setDesc("Theme 2");

  //---

  movePalette("palette2", 0);
  movePalette("palette1", 1);
  movePalette("set2"    , 2);

  //---

  selectColor_ = QColor("#44ee44");
  insideColor_ = QColor("#4444ee");
}
