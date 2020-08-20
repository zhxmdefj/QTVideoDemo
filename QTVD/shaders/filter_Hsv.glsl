uniform sampler2D inputImageTexture;
varying highp vec2 textureCoordinate;
uniform float u_range_left;//色相范围左
uniform float u_range_right;//色相范围右
uniform float u_stander;
uniform float u_range;
uniform float u_offset;
uniform float u_hue;
uniform float u_saturation;
uniform float u_value;
uniform float u_contrast;
vec3 rgbtohsv(vec3 rgb)
{
    float R = rgb.x;
    float G = rgb.y;
    float B = rgb.z;
    vec3 hsv;
    float max1 = max(R, max(G, B));
    float min1 = min(R, min(G, B));
    if (R == max1) {
        hsv.x = (G - B) / (max1 - min1);
    }
    if (G == max1) {
        hsv.x = 2.0 + (B - R) / (max1 - min1);
    }
    if (B == max1) {
        hsv.x = 4.0 + (R - G) / (max1 - min1);
    }
    hsv.x = hsv.x * 60.0;
    if (hsv.x  < 0.0) {
        hsv.x = hsv.x + 360.0;
    }
    hsv.z = max1;
    hsv.y = (max1 - min1) / max1;
    return hsv;
}
vec3 hsvtorgb(vec3 hsv)
{
    float R;
    float G;
    float B;
    if (hsv.y == 0.0)
    {
        R = G = B = hsv.z;
    }
    else
    {
        hsv.x = hsv.x / 60.0;
        int i = int(hsv.x);
        float f = hsv.x - float(i);
        float a = hsv.z * (1.0 - hsv.y);
        float b = hsv.z * (1.0 - hsv.y * f);
        float c = hsv.z * (1.0 - hsv.y * (1.0 - f));
        if (i == 0)
        {
            R = hsv.z;
            G = c;
            B = a;
        }
        else if (i == 1)
        {
            R = b;
            G = hsv.z;
            B = a;
        }
        else if (i == 2)
        {
            R = a;
            G = hsv.z;
            B = c;
        }
        else if (i == 3)
        {
            R = a;
            G = b;
            B = hsv.z;
        }
        else if (i == 4)
        {
            R = c;
            G = a;
            B = hsv.z;
        }
        else
        {
            R = hsv.z;
            G = a;
            B = b;
        }
    }
    return vec3(R, G, B);
}

vec3 hueAdjust(vec3 hsb,vec3 maskHsb,float standHue,float deltaRange,vec3 hueParam,float offset)
{
    if ((hueParam.x==50.0) &&(hueParam.y==50.0) &&(hueParam.z==50.0))
    return hsb;
    //hsb adjust
    vec3 tmpHSB;
    float fAlpha;
    float standHue0,minHue,maxHue,currHue;
    //Red
    hueParam.x=(hueParam.x - 50.0)*30.0/50.0;
    hueParam.y=(hueParam.y - 50.0)*0.8/50.0;
    hueParam.z=(hueParam.z - 50.0)/50.0;

    //init data
    standHue0=standHue+offset;
    minHue=standHue0-deltaRange;
    maxHue=standHue0+deltaRange;
    tmpHSB=hsb;
    //check range

    if (offset>0.0)
    {
        if (maskHsb.r>180.0)
        currHue=maskHsb.r-360.0+offset;
        else
        currHue=maskHsb.r+offset;
    }
    else
        currHue=maskHsb.r;


    if ((currHue>=minHue) && (currHue <= maxHue))
    {
        //get alpha
        fAlpha=abs(currHue-standHue0);
        //fAlpha=1.0-fAlpha/deltaRange;
        fAlpha=1.0-fAlpha/deltaRange;

        //hue
        tmpHSB.x=hsb.r+hueParam.x*fAlpha;
        if (tmpHSB.x>360.0)
        tmpHSB.x=tmpHSB.x-360.0;
        //saturation
        tmpHSB.y=hsb.y+hsb.y*hueParam.y*fAlpha;
        tmpHSB.y=clamp(tmpHSB.y,0.0,1.0);
        //bright
        tmpHSB.z=hsb.z+hsb.z*hsb.y*hueParam.z*fAlpha;
        //tmpHSB.z=hsb.z+hsb.z*hsb.y*hueParam.z*0.5; //
        tmpHSB.z=clamp(tmpHSB.z,0.0,1.0);
    }

    return tmpHSB;
}

void main()
{
    vec4 pixColor = texture2D(inputImageTexture, textureCoordinate);
//    vec3 hsv;
//    float range = 0.0;//色调调整的范围
//    float standHue0 = 0.0;
////    u_hue = u_hue;
////    u_value = u_value - 50.0;
////    u_saturation = u_saturation - 50.0;
//
//    float hue = (u_hue - 50.0) * 30.0 / 50.0;
//    float value = (u_value - 50.0) * 0.8 / 50.0;
//    float saturation = (u_saturation - 50.0) / 50.0;
//    hsv.xyz = rgbtohsv(pixColor.rgb);
//
//    float fAlpha=abs(hsv.x - standHue0);
//    fAlpha = 1.0 -  fAlpha / (range / 2.0);
//
//    if (u_offset > 0.0) {
//        if (hsv.x > 180.0)
//            hsv.x = hsv.x - 360.0 + u_offset;
//        else
//            hsv.x = hsv.x + u_offset;
//    }
//
//    float stander = u_stander + u_offset;
//    float left = stander - u_range;
//    float right = stander + u_range;
//    vec3 tmpHsv;
//    //左边界小于右边界
//    if (u_range_left < u_range_right)
//    {
//
//        if (hsv.x >= u_range_left && hsv.x < u_range_right)
//        {
//            tmpHsv.x = hsv.x + hue * fAlpha;
//            if (tmpHsv.x < 0.0)
//            tmpHsv.x += 360.0;
//
//            if (tmpHsv.x > 360.0)
//            tmpHsv.x -= 360.0;
//
//            tmpHsv.z = hsv.z + hsv.z * hsv.y * value * fAlpha;
//            tmpHsv.z = clamp(tmpHsv.z, 0.0, 1.0);
//            tmpHsv.y = hsv.y + hsv.y * saturation * fAlpha;
//            tmpHsv.y = clamp(tmpHsv.y, 0.0, 1.0);
//
//            vec3 f_color = hsvtorgb(tmpHsv);
////            f_color = ((f_color - 0.5) * max(contrast+1.0, 0.0)) + 0.5;
//            gl_FragColor =  pixColor * vec4(f_color, pixColor.a);
//        }
//        else
//        {
//            gl_FragColor =  pixColor;
//        }
//        //gl_FragColor =  vec4(0.0, 0.7, 0.3, 1.0);
//    }
//    else if(u_range_left > u_range_right)  //右边界小于左边界
//    {
//        if ((hsv.x > u_range_left || hsv.x < u_range_right))
//        {
//            tmpHsv.x = hsv.x + hue * fAlpha;
//            if (tmpHsv.x < 0.0)
//            tmpHsv.x += 360.0;
//
//            if (tmpHsv.x > 360.0)
//            tmpHsv.x -= 360.0;
//
//            tmpHsv.z = hsv.z + hsv.z * hsv.y * value * fAlpha;
//            tmpHsv.z = clamp(tmpHsv.z, 0.0, 1.0);
//            tmpHsv.y = hsv.y + hsv.y * saturation * fAlpha;
//            tmpHsv.y = clamp(tmpHsv.y, 0.0, 1.0);
//
//            vec3 f_color = hsvtorgb(tmpHsv);
//            //            f_color = ((f_color - 0.5) * max(contrast+1.0, 0.0)) + 0.5;
//            gl_FragColor =  pixColor * vec4(f_color, pixColor.a);
//        }
//        else
//        {
//            gl_FragColor =  pixColor;
//        }
////        gl_FragColor =  vec4(0.5, 0.2, 0.3, 1.0);
//    }
//    else
//    {
//        gl_FragColor =  pixColor;
//    }
    vec3 hsvv = rgbtohsv(pixColor.rgb);
    gl_FragColor = vec4(hsvtorgb(hueAdjust(hsvv, hsvv, u_stander, u_range, vec3(u_hue, u_saturation, u_value), u_offset)), 1.0);
//    gl_FragColor =  hsvtorgb(rgbtohsv(pixColor));
//    gl_FragColor = vec4(vec3(u_hue / 100.0), 1.0);
}