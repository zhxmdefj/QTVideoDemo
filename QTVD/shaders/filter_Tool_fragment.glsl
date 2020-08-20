precision mediump float;
varying mediump vec2 textureCoordinate;
uniform sampler2D inputImageTexture;
uniform sampler2D uSkinLookupTable;
//uniform sampler2D ColorBalance;
uniform mediump float Shadow;       //阴影
uniform mediump float Highlight;     //高光
uniform mediump float Contrast;     //对比度
uniform mediump float Saturation;   //饱和度
uniform mediump float Vibrance;    //自然饱和度
uniform mediump float WhiteSkin;        //美肤
uniform mediump float Exposure;         //曝光
uniform mediump float Temperature;      //色温
uniform mediump float Hue;          //色相

const mediump vec3 lumCoeff=vec3(0.2125, 0.7154, 0.0721);

mediump float BlendSoftLightf(mediump float base, mediump float blend)
{
    mediump float fValue=base;
    if (blend < 0.5)
    fValue=base * blend*2.0 + base * base * (1.0 - 2.0 * blend);
    else
    fValue=sqrt(base) * (2.0 * blend - 1.0) + 2.0 * base * (1.0 - blend);
    return fValue;
}

mediump vec3 blendSoftLight(mediump vec3 base, mediump vec3 blend)
{
    mediump vec3 result;
    result.r=BlendSoftLightf(base.r, blend.r);
    result.g=BlendSoftLightf(base.g, blend.g);
    result.b=BlendSoftLightf(base.b, blend.b);
    return result;
}

mediump vec3 rgb2lab(mediump vec3 rgb)
{
    mediump vec3 lab;
    lab.x = 0.2126 * rgb.r + 0.7152 * rgb.g + 0.0722 * rgb.b;
    lab.y = 1.4749 * (0.2213 * rgb.r - 0.3390 * rgb.g + 0.1177 * rgb.b) + 0.5;
    lab.z = 0.6245 * (0.1949 * rgb.r + 0.6057 * rgb.g - 0.8006 * rgb.b) + 0.5;
    return lab;
}

mediump vec3 lab2rgb(mediump vec3 lab)
{
    mediump float L1, a1, b1;

    L1=lab.x;
    a1=(lab.y-0.5)*0.682353;
    b1=(lab.z-0.5)*1.607843;

    mediump vec3 rgb;
    rgb.r=L1+(a1*3.07989502+b1*0.542907715);
    rgb.g=L1-(a1*0.92089844+b1*0.045196535);
    rgb.b=L1+(a1*0.05310059-b1*1.151092529);

    rgb=clamp(rgb, 0.0, 1.0);
    return rgb;
}


//////////// nature saturation begin ///////////////

vec3 DoRGBtoMMH (vec3 rgb)
{
    float r = rgb . r;
    float g = rgb . g;
    float b = rgb . b;
    bool mirror0 = (b > g);
    if (mirror0)
    {
        float temp = g;
        g = b;
        b = temp;
    }
    bool mirror1 = (b > r);
    if (mirror1)
    {
        float temp = r;
        r = b;
        b = temp;
    }
    bool mirror2 = (g > r);
    if (mirror2)
    {
        float temp = g;
        g = r;
        r = temp;
    }
    float hue;
    if (r == b)
    {
        hue = 0.0;
    }
    else
    {
        hue = (g - b) / (r - b);
    }
    if (mirror2)
    {
        hue = 2.0 - hue;
    }
    if (mirror1)
    {
        hue = 4.0 - hue;
    }
    if (mirror0)
    {
        hue = 6.0 - hue;
    }
    return vec3 (b, r, hue);
}


vec3 DoMMHtoRGB (vec3 mmh)
{
    float hue = mmh . z;
    float r = mmh . y;
    float b = mmh . x;
    if (hue > 6.0)
    {
        hue -= 6.0;
    }
    /*// Error in mz4, i don't konw reason.
    if ( hue > 6.0 )
    {
    hue -= 6.0 ;
    }*/

    bool mirror0 = (hue > 3.0);
    if (mirror0)
    {
        hue = 6.0 - hue;
    }
    bool mirror1 = (hue > 2.0);
    if (mirror1)
    {
        hue = 4.0 - hue;
    }
    bool mirror2 = (hue > 1.0);
    if (mirror2)
    {
        hue = 2.0 - hue;
    }
    float g = b + (r - b) * hue;
    if (mirror2)
    {
        float temp = g;
        g = r;
        r = temp;
    }
    if (mirror1)
    {
        float temp = r;
        r = b;
        b = temp;
    }
    if (mirror0)
    {
        float temp = g;
        g = b;
        b = temp;
    }
    return vec3 (r, g, b);
}

const float kMinSkinHue = - 60.0;
const float kMinCentralSkinHue = 0.0;
const float kMaxCentralSkinHue = 30.0;
const float kMaxSkinHue = 45.0;
const float kMinScaledSkinHue = (kMinSkinHue + 60.0) / 60.0;
const float kMinScaledCentralSkinHue = (kMinCentralSkinHue + 60.0) / 60.0;
const float kMaxScaledCentralSkinHue = (kMaxCentralSkinHue + 60.0) / 60.0;
const float kMaxScaledSkinHue = (kMaxSkinHue + 60.0) / 60.0;
const float kMinSideFalloff = 1.0 / (kMinScaledCentralSkinHue - kMinScaledSkinHue);
const float kMaxSideFalloff = 1.0 / (kMaxScaledSkinHue - kMaxScaledCentralSkinHue);
vec3 DoIncreaseVibrance (vec3 minMaxHue, float vibranceShift)
{
    const float kMaxSaturationGain = 6.0;
    const float kMaxSkinSaturationGain = 1.75;
    const float kStandardFactor = 1.0 - (1.0 / kMaxSaturationGain);
    const float kSkinFactor = 1.0 - (1.0 / kMaxSkinSaturationGain);
    const float kSkinProtectionSlope = kSkinFactor - kStandardFactor;
    float minValue = minMaxHue . x;
    float maxValue = minMaxHue . y;
    float gap = maxValue - minValue;
    if (gap == 0.0)
    {
        return minMaxHue;
    }
    float saturation = gap / maxValue;
    float shiftFactor = vibranceShift;
    float hue = minMaxHue . z;
    float shiftedHue = hue + 1.0;
    if (6.0 <= shiftedHue) shiftedHue -= 6.0;
    float minSide = clamp ((1.0 - (kMinScaledCentralSkinHue - shiftedHue) * kMinSideFalloff), 0.0, 1.0);
    float maxSide = clamp ((1.0 - (shiftedHue - kMaxScaledCentralSkinHue) * kMaxSideFalloff), 0.0, 1.0);
    float protection = min (minSide, maxSide);
    protection *= (1.0 - saturation * saturation);
    shiftFactor = shiftFactor * (shiftFactor * protection + 1.0 - protection);
    float saturationFactor = kStandardFactor + kSkinProtectionSlope * protection;
    saturationFactor *= shiftFactor;
    saturationFactor *= 1.0 - minValue;
    float shadowRolloff = min (1.0, 16.0 * maxValue);
    shadowRolloff *= 2.0 - shadowRolloff;
    saturationFactor *= shadowRolloff;
    float saturationMask = saturation * (1.0 - saturation);
    saturationMask *= 2.0 - saturationMask;
    float brightnessFactor = shiftFactor * shadowRolloff * saturationMask;
    saturation = saturation / (1.0 - saturationFactor * (1.0 - saturation));
    maxValue *= 1.0 + 0.25 * brightnessFactor * (1.0 - maxValue);
    minValue = maxValue - saturation * maxValue;
    return vec3 (minValue, maxValue, hue);
}

vec3 DoDecreaseVibrance (vec3 minMaxHue, float vibranceShift)
{
    float minValue = minMaxHue . x;
    float maxValue = minMaxHue . y;
    float gap = maxValue - minValue;
    if (gap == 0.0)
    {
        return minMaxHue;
    }
    float saturation = gap / maxValue;
    float saturationFactor = - vibranceShift;
    float shadowRolloff = min (1.0, 16.0 * maxValue);
    shadowRolloff *= 2.0 - shadowRolloff;
    float saturationMask = saturation * (1.0 - saturation);
    saturationMask *= 2.0 - saturationMask;
    float brightnessFactor = - vibranceShift * shadowRolloff * saturationMask;
    saturation *= 1.0 - saturationFactor + saturationFactor * saturation * (0.5 + 0.5 * saturation) * shadowRolloff;
    saturation *= 1.0 - 0.25 * saturationFactor;
    maxValue *= 1.0 - brightnessFactor * (1.0 - maxValue);
    minValue = maxValue - saturation * maxValue;
    return vec3 (minValue, maxValue, minMaxHue . z);
}

vec3 DoVibrance (vec3 rgb, float vibranceValue)
{
    if (vibranceValue > 0.0)
    {
        rgb = DoIncreaseVibrance (rgb, vibranceValue);
    }
    else
    {
        rgb = DoDecreaseVibrance (rgb, vibranceValue);
    }
    return rgb;
}


vec3 DoVibranceHSL (vec3 rgb, float vibranceValue)
{

    // Go to min-max-hue space.
    vec3 mmh = DoRGBtoMMH (rgb);

    // Vibrance.
    mmh = DoVibrance (mmh, vibranceValue);

    // Back to RGB space.
    rgb = DoMMHtoRGB (mmh);

    return rgb;
}

/////// nature saturation end   //////////

float gethsv_s(vec3 rgb)
{
    float rc = rgb.r;
    float gc = rgb.g;
    float bc = rgb.b;

    float h = 0.0;
    float s = 0.0;
    float v = 0.0;

    float max_v = max(rc, max(gc, bc));
    float min_v = min(rc, min(gc, bc));
    float delta = max_v - min_v;

    v = max_v;

    if (max_v != 0.0) {
        s = delta / max_v;
    } else {
        s = 0.0;
    }
    return s;
}

void main()
{
    mediump vec3 clO, clA, clB, clC, clBlur, clTmp;
    mediump float lum, lumBlur, fTmp, alpha;
    mediump float result, base, blend;
    mediump float fTmpGamma;


    mediump vec4 baseColor=texture2D(inputImageTexture, textureCoordinate);
    clO=baseColor.rgb;
    clA=clO;

    //1.temperature
    clO=clA;
    mediump vec3 t=clO;
    mediump vec3 s = t;
    mediump vec3 oneMinusT = 1.0 - t;
    mediump float p, p1, lambda;
    if (Temperature> 50.0)
    {
        p=Temperature/50.0 - 1.0;
        p1 = p + 1.0;
        lambda=0.6;
        mediump vec3 tPart = lambda*p*t*(1.0 - t*t)/(p1*p1);
        s.b = pow (t.b, p1)  + tPart.b;
        s.r = (1.0 - pow(oneMinusT.r, p1) - tPart.r);
        s=clamp(s, 0.0, 1.0);
        clA=s;
    }
    else
    if (Temperature < 50.0)
    {
        p=-(Temperature/50.0 - 1.0);
        p1 = p + 1.0;
        lambda=0.6;
        mediump vec3 tPart = lambda*p*t*(1.0 - t*t)/(p1*p1);
        s.r = pow (t.r, p1)  + tPart.r;
        s.b = (1.0 - pow(oneMinusT.b, p1) - tPart.b);
        s=clamp(s, 0.0, 1.0);
        clA=s;
    }

    //2.Hue
    clO=clA;
    t=clO;
    s = t;
    oneMinusT = 1.0 - t;
    if (Hue > 50.0)
    {
        p=Hue / 50.0 - 1.0;
        p1 = p + 1.0;
        lambda=0.6;
        mediump vec3 tPart = lambda*p*t*(1.0 - t*t)/(p1*p1);
        s.g = pow (t.g, p1)  + tPart.g;
        s.r = (1.0 - pow(oneMinusT.r, p1) - tPart.r);
        s=clamp(s, 0.0, 1.0);
        clA=s;
    }
    else
    if (Hue < 50.0)
    {
        p=-(Hue / 50.0 - 1.0);
        p1 = p + 1.0;
        lambda=0.6;
        mediump vec3 tPart = lambda*p*t*(1.0 - t*t)/(p1*p1);
        s.r = pow (t.r, p1)  + tPart.r;
        s.g = (1.0 - pow(oneMinusT.g, p1) - tPart.g);
        s=clamp(s, 0.0, 1.0);
        clA=s;
    }

    //3.exposure
    //                clO=clA;
    //                lumBlur=dot(clO,lumCoeff);
    //                clA=clO * pow(2.0, Exposure);
    //                clA=clamp(clA,0.0,1.0);
    clO=clA;
    if (Exposure >= 50.0)
    {
        lumBlur=1.0-dot(clO, lumCoeff);
        clA=clO * (pow(2.0, (Exposure/50.0 - 1.0) *lumBlur));
        clA=clamp(clA, 0.0, 1.0);
    }
    else
    {
        lumBlur=dot(clO, lumCoeff);
        clA=clO * (pow(2.0, (Exposure/50.0 - 1.0)));
        clA=clamp(clA, 0.0, 1.0);
    }


    //4.Contrast
    clO=clA;
    lumBlur=dot(clO, lumCoeff);
    clA=clO+(clO-0.5)*(1.0/(1.0-(Contrast/50.0 - 1.0)*0.5)-1.0);
    clA=clamp(clA, 0.0, 1.0);
    clA=mix(clO, clA, 1.0-sin(lumBlur*3.1415926/2.0));


    //5.shadow and highlight
    clO=clA;
    lumBlur=dot(clO, lumCoeff);
    lumBlur=lumBlur*2.0-1.0;



    if (lumBlur < 0.0)
    {
        mediump float factor = 1.0;
        if (Shadow > 50.0)
        {
            factor = 0.7;
        }
        fTmpGamma = pow(10.0, abs(lumBlur) * (Shadow - 50.0)/100.0*factor);
        clA=vec3(1.0 - pow(vec3(1.0 - clO), vec3(fTmpGamma)));
    }
    else
    //Highlight
    {
        mediump float factor = 1.0;
        if (Highlight < 50.0)
        {
            factor = 0.5;
        }
        fTmpGamma=pow(10.0, abs(lumBlur) * ((-(Highlight - 50.0))/100.0) * factor);
        clA=pow(clO, vec3(fTmpGamma));
    }


    //Vibrance old
    /*
    clO=clA;
    float ss;
    ss=1.0-gethsv_s(clO);
    ss=sin(ss*1.57);
    lumBlur=dot(clA,lumCoeff);
    clB=mix(vec3(lumBlur),clO,Vibrance/100.0+1.0);
    clA=mix(clA,clB,ss);*/


    // Vibrance new(pertect skin)
    float fVibrance = Vibrance>=0.0?(Vibrance/50.0 - 1.0)*1.2:(Vibrance/50.0 - 1.0)*1.2;
    clA=DoVibranceHSL(clA, fVibrance);


    //Saturation
    clO=clA;
    lumBlur=dot(clA, lumCoeff);
    clA=mix(vec3(lumBlur), clO, (Saturation/50.0));

    clO = clA;

    //whitting skin
//    clA.r = texture2D(uSkinLookupTable, vec2(clA.r, 0.5)).r;
//    clA.g = texture2D(uSkinLookupTable, vec2(clA.g, 0.5)).g;
//    clA.b = texture2D(uSkinLookupTable, vec2(clA.b, 0.5)).b;
//    clA = mix(clO,clA,abs(WhiteSkin));
//
//    //8.curve
//    clO=clA;
//    clA.r=texture2D(Curve, vec2(clO.r, 0.5)).r;
//    clA.g=texture2D(Curve, vec2(clO.g, 0.5)).g;
//    clA.b=texture2D(Curve, vec2(clO.b, 0.5)).b;
//
//
//    clA.r=texture2D(Curve, vec2(clA.r, 0.5)).a;
//    clA.g=texture2D(Curve, vec2(clA.g, 0.5)).a;
//    clA.b=texture2D(Curve, vec2(clA.b, 0.5)).a;
//
//    //9.ColorBalance
//    clA.r=texture2D(ColorBalance, vec2(clA.r, 0.5)).r;
//    clA.g=texture2D(ColorBalance, vec2(clA.g, 0.5)).g;
//    clA.b=texture2D(ColorBalance, vec2(clA.b, 0.5)).b;

    gl_FragColor=vec4(clA, baseColor.a);
}
            