float g_ScreenWidth;
float g_ScreenHeight;

float ZNear;
float ZFar;

float TransformZ(float zv, float f_znear, float f_zfar, float fov, float phd_winwidth)
{
    float MINZR = 0.005f;
    float MAXZR = 0.995f;
    float ZRANGE = (MAXZR - MINZR);

    fov /= 2.0f;
    float f_persp = ((phd_winwidth / 2.0f) * cos(fov)) / sin(fov);

    float f_b = (ZRANGE * f_znear * f_zfar) / (f_znear - f_zfar);
    f_b = -f_b; // как в оригинале движка
    float f_a = MINZR - (f_b / f_znear);

    float p_sz = f_a - (f_b * (zv / f_persp));
    return p_sz;
}

//pass 0 
//текстурированные цветные (освещенные) полигоны (не прозрачные)

struct VSInput
{
    float4 pos : POSITION;  // x, y, z, w (уже в screen-space)
    float2 tex : TEXCOORD;
    float4 Color : COLOR0;    // цвет 0..255
};

struct PSInput
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
    float rhw : TEXCOORD1; // RHW для перспективной коррекции
    float4 Color : COLOR0;
};


PSInput vs_main_tex_color(VSInput input)
{
    PSInput output;

    float Q = ZFar / (ZFar - ZNear);

    //переводим экранные координаты в диапазон шейдера -1 и +1
    output.pos.x = (2.0f * input.pos.x / g_ScreenWidth) - 1.0f;
    output.pos.y = 1.0f - (2.0f * input.pos.y / g_ScreenHeight);
    //output.pos.z = input.pos.z;

    //#1перспективный z буфер глюки с аптечками и спрайтами
    //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    
    //#2линейный z буфер нет глюков с аптечками и спрайтами
    output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    
    //#3взято z буффер из TR2 расчет
    //output.pos.z = TransformZ(input.pos.z, ZNear, ZFar, 80, g_ScreenWidth);

    output.pos.w = 1;       
    //клиппинг в экранных координатах делать когда tex * 1/w
    //поэтому ниже закоментировано (смотри cpp код tex * 1/w)
    //output.tex = input.tex * input.pos.w;   
    output.tex.xy = input.tex.xy;   
    output.rhw = input.pos.w; 

    output.Color = input.Color;

    return output;
}

sampler TextureScreen;

float4 ps_main_tex_color (PSInput input) : COLOR
{
    float rhw = 1.0f / input.rhw;

    float2 tex = input.tex * rhw ;

    float4 texColor = tex2D(TextureScreen, tex);

    float4 finalColor = texColor * input.Color;

    return float4(finalColor.rgb, texColor.a);
}


//-----------------------------------------------
//pass 1
//цветные полигоны (не текстурированные, не прозрачные)

struct VSInput2
{
    float4 pos : POSITION;  // x, y, z, w (уже в screen-space)
    float4 Color : COLOR0;    // цвет 0..255
};

struct PSInput2
{
    float4 pos : POSITION;
    float4 Color : COLOR0;
};

PSInput2 vs_main_color(VSInput2 input)
{
    PSInput2 output;

    float Q = ZFar / (ZFar - ZNear);

    //переводим экранные координаты в диапазон шейдера -1 и +1
    output.pos.x = (2.0f * input.pos.x / g_ScreenWidth) - 1.0f;
    output.pos.y = 1.0f - (2.0f * input.pos.y / g_ScreenHeight);
    //output.pos.z = input.pos.z; 

    //#1перспективный z буфер глюки с аптечками и спрайтами
    //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    
    //#2линейный z буфер нет глюков с аптечками и спрайтами
    output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    
    //#3взято z буффер из TR2 расчет
    //output.pos.z = TransformZ(input.pos.z, ZNear, ZFar, 80, g_ScreenWidth);
    
    output.pos.w = 1;       

    output.Color = input.Color;

    return output;
}



float4 ps_main_color (PSInput2 input) : COLOR
{
    
    float4 finalColor = input.Color;

    return finalColor;
    
}

//----------------------------------
//pass 2
//прозрачные полигоны текстурированные цветные (с альфой)
//то есть это спрайты (аптечки например), заросли на потолке зеленые,
//так же текст в титлах и фпс и другой текст

PSInput vs_main_transparent(VSInput input)
{
    PSInput output;

    float Q = ZFar / (ZFar - ZNear);

    //переводим экранные координаты в диапазон шейдера -1 и +1
    output.pos.x = (2.0f * input.pos.x / g_ScreenWidth) - 1.0f;
    output.pos.y = 1.0f - (2.0f * input.pos.y / g_ScreenHeight);
    //output.pos.z = input.pos.z;

    //#1перспективный z буфер глюки с аптечками и спрайтами
    //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    
    //#2линейный z буфер нет глюков с аптечками и спрайтами
    output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    
    //#3взято z буффер из TR2 расчет
    //output.pos.z = TransformZ(input.pos.z, ZNear, ZFar, 80, g_ScreenWidth);
    
    output.pos.w = 1;       
    //клиппинг в экранных координатах делать когда tex * 1/w
    //поэтому ниже закоментировано (смотри cpp код tex * 1/w)
    //output.tex = input.tex * input.pos.w;   
    output.tex.xy = input.tex.xy;   
    output.rhw = input.pos.w; 

    output.Color = input.Color;

    return output;
}

sampler TextureScreen2;

float4 ps_main_transparent (PSInput input) : COLOR
{
    float rhw = 1.0f / input.rhw;

    float2 tex = input.tex * rhw ;

    float4 col = input.Color * rhw;

    float4 texColor = tex2D(TextureScreen2, tex);

    // умножаем на цвет вершины
    float4 finalColor = texColor * input.Color;

    return float4(finalColor.rgb, texColor.a);
    
}

//-------------------------------
//pass 4
//рисование линий - например в титлах box из линий
//например монастырь линия на молнии в комнате тора

struct VS_INPUT_LINES
{
    float4 pos   : POSITION;
    float4 Color : COLOR0;
};

struct VS_OUTPUT_LINES
{
    float4 pos   : POSITION;
    float4 Color : COLOR0;
};


VS_OUTPUT_LINES vs_main_lines(VS_INPUT_LINES input)
{
    VS_OUTPUT_LINES output;
    
    //переводим экранные координаты в диапазон шейдера -1 и +1
    output.pos.x = (2.0f * input.pos.x / g_ScreenWidth) - 1.0f;
    output.pos.y = 1.0f - (2.0f * input.pos.y / g_ScreenHeight);
    //output.pos.z = input.pos.z;

    //#1перспективный z буфер глюки с аптечками и спрайтами
    //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    
    //#2линейный z буфер нет глюков с аптечками и спрайтами
    output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    
    //#3взято z буффер из TR2 расчет
    //output.pos.z = TransformZ(input.pos.z, ZNear, ZFar, 80, g_ScreenWidth);
    
    output.pos.w = 1;       

    output.Color = input.Color;


    return output;
}

float4 ps_main_lines (VS_OUTPUT_LINES input) : COLOR
{
    return input.Color;
}

//--------------------------------
//pass 5
//прозрачный цветной четырехугольник в титлах load/save бакгроунд под текстом

struct VSInput_transquad
{
    float4 pos : POSITION;  // x, y, z, w (уже в screen-space)
    float4 Color : COLOR0;    // цвет 0..255
};

struct PSInput_transquad
{
    float4 pos : POSITION;
    float4 Color : COLOR0;
};

PSInput_transquad vs_main_transquad(VSInput_transquad input)
{
    PSInput_transquad output;

    float Q = ZFar / (ZFar - ZNear);

    //переводим экранные координаты в диапазон шейдера -1 и +1
    output.pos.x = (2.0f * input.pos.x / g_ScreenWidth) - 1.0f;
    output.pos.y = 1.0f - (2.0f * input.pos.y / g_ScreenHeight);
    //output.pos.z = input.pos.z; 

    //#1перспективный z буфер глюки с аптечками и спрайтами
    //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    
    //#2линейный z буфер нет глюков с аптечками и спрайтами
    output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    
    //#3взято z буффер из TR2 расчет
    //output.pos.z = TransformZ(input.pos.z, ZNear, ZFar, 80, g_ScreenWidth);

    
    output.pos.w = 1;       

    output.Color = input.Color;

    return output;
}



float4 ps_main_transquad (PSInput_transquad input) : COLOR
{

    float4 finalColor = input.Color;

    return finalColor;
    
}


technique TexturedTech
{
	pass P0
    {
        vertexShader = compile vs_2_0 vs_main_tex_color();
        pixelShader  = compile ps_2_0 ps_main_tex_color();
    }

    pass P1
    {
        vertexShader = compile vs_2_0 vs_main_color();
        pixelShader  = compile ps_2_0 ps_main_color();
    }


    pass P2
    {
        vertexShader = compile vs_2_0 vs_main_transparent();
        pixelShader  = compile ps_2_0 ps_main_transparent();
    }

   pass P3
    {
        vertexShader = compile vs_2_0 vs_main_lines();
        pixelShader  = compile ps_2_0 ps_main_lines();
    }

    pass P4
    {
        vertexShader = compile vs_2_0 vs_main_transquad();
        pixelShader  = compile ps_2_0 ps_main_transquad();
    }

}