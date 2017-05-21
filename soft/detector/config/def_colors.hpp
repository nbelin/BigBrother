/*
 * LUV
 */
Color LUV_g(200, 66, 161);
int LUV_gr = 30;

Color LUV_b(47, 95, 85);
int LUV_br = 30;

Color LUV_m(105, 140, 130);
int LUV_mr = 20;


/*
 * HSV
 */
Color HSV_g_min(30, 50, 50);
Color HSV_g_max(60, 255, 255);
Color HSV_b_min(95, 50, 50);
Color HSV_b_max(120, 255, 255);
Color HSV_m_min(145, 50, 50);
Color HSV_m_max(175, 255, 255);



/*
 * Markers
 */
//ColorSet redSet(Color(148, 127, 107), 30);
//ColorSet yellowSet(Color(202, 103, 208), 40);
//ColorSet blueSet(Color(200, 59, 86), 30);
ColorSet greenSet(LUV_g, LUV_gr, HSV_g_min, HSV_g_max);
ColorSet darkBlueSet(LUV_b, LUV_br, HSV_b_min, HSV_b_max);
ColorSet magentaSet(LUV_m, LUV_mr, HSV_m_min, HSV_m_max);

// marker 1: MGB (our robot)
// marker 2: MBG (our robot)
// marker 3: GMB (other)
// marker 4: GBM (other)
Marker getMarker(const Image3D& firstImage, int id) {
    switch (id) {
    case 1:
        return Marker(firstImage, false, magentaSet, greenSet, darkBlueSet);
    case 2:
        return Marker(firstImage, false, magentaSet, darkBlueSet, greenSet);
    case 3:
        return Marker(firstImage, true, greenSet, magentaSet, darkBlueSet);
    case 4:
        return Marker(firstImage, true, greenSet, darkBlueSet, magentaSet);
    default:
        exit(-1);
    }
}
