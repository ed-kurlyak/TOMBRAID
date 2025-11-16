float g_ScreenWidth;
float g_ScreenHeight;

float ZNear;
float ZFar;

float Fov = 80.0f * 3.1415926 / 180.0f;


int ZBuff_Persp = 1;
int ZBuff_Linear = 2;
int ZBuff_TR2 = 3;

int ZBuff_Mode = 2;

float TransformZ(float zv, float f_znear, float f_zfar, float fov_rad, float screen_width)
{
    // минимальные и максимальные значения Z для нормализации
    const float MINZR = 0.005f;
    const float MAXZR = 0.995f;
    const float ZRANGE = MAXZR - MINZR;

    float one = (256.0f * 8.0f * 16384.0f);

    // делим FOV на 2, как в перспективной формуле
    fov_rad /= 2.0f;

    // расчет перспективного коэффициента
    float f_persp = ((screen_width / 2.0f) * cos(fov_rad)) / sin(fov_rad);

    // инвертируем Z относительно перспективы
    float ooz = f_persp / zv;

    // коэффициент для масштабирования Z
    //float f_oneopersp = 1.0f / f_persp;
    float f_oneopersp = one / f_persp;

    // нормализованное значение с учетом перспективы
    ooz *= f_oneopersp;

    // расчёт стандартных коэффициентов движка
    float f_b = (ZRANGE * f_znear * f_zfar) / (f_znear - f_zfar);
    float f_a = MINZR - (f_b / f_znear);
    f_b = -f_b;

    
    // масштабирование с учётом инверсии ooz
    //float f_boo = f_b / 1.0f;
    float f_boo = f_b / one;

    float p_sz = f_a - f_boo * ooz;


    return p_sz;
    
}

float TransformZ_Sprite(float zv, float f_znear, float f_zfar, float fov_rad, float screen_width)
{
    // минимальные и максимальные значения Z для нормализации
    const float MINZR = 0.005f;
    const float MAXZR = 0.995f;
    const float ZRANGE = MAXZR - MINZR;

    float one = (256.0f * 8.0f * 16384.0f);

    float ooz = one / zv;

    // расчёт стандартных коэффициентов движка
    float f_b = (ZRANGE * f_znear * f_zfar) / (f_znear - f_zfar);
    float f_a = MINZR - (f_b / f_znear);
    f_b = -f_b;

    
    // масштабирование с учётом инверсии ooz
    float f_boo = f_b / one;
    float p_sz = f_a - f_boo * ooz;

    return p_sz;

    
}


/*
float TransformZ(float zv, float f_znear, float f_zfar, float fov_rad, float screen_width)
{
    const float MINZR = 0.005f;
    const float MAXZR = 0.995f;
    const float ZRANGE = MAXZR - MINZR;
    
    // zv приходит в fixed-point формате (<<14)
    const float FIXED_SCALE = 16384.0f;
    
    f_znear = f_znear / FIXED_SCALE;
    f_zfar = f_zfar / FIXED_SCALE;
    float zf = zv / FIXED_SCALE;

    fov_rad *= 0.5f;

    float f_persp = ((screen_width * 0.5f) * cos(fov_rad)) / sin(fov_rad);

    // классическая перспектива
    float ooz = f_persp / zf;

    float f_oneopersp = 1.0f / f_persp;
    ooz *= f_oneopersp;

    float f_b = (ZRANGE * f_znear * f_zfar) / (f_znear - f_zfar);
    float f_a = MINZR - (f_b / f_znear);
    f_b = -f_b;

    float p_sz = f_a - (f_b * ooz);

    return p_sz;
}
*/

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

    if(ZBuff_Mode == ZBuff_Persp)
    {
        //#1перспективный z буфер глюки с аптечками и спрайтами
        //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    }
    
    else if(ZBuff_Mode == ZBuff_Linear)
    {
        //#2линейный z буфер нет глюков с аптечками и спрайтами
        output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    }
    
    else if(ZBuff_Mode == ZBuff_TR2)
    {
        //#3взято z буффер из TR2 расчет
        output.pos.z = TransformZ(input.pos.z, ZNear, ZFar, Fov, g_ScreenWidth);
    }

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

    if(ZBuff_Mode == ZBuff_Persp)
    {
        //#1перспективный z буфер глюки с аптечками и спрайтами
        //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    }
    
    else if(ZBuff_Mode == ZBuff_Linear)
    {
        //#2линейный z буфер нет глюков с аптечками и спрайтами
        output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    }
    
    else if(ZBuff_Mode == ZBuff_TR2)
    {
        //#3взято z буффер из TR2 расчет
        output.pos.z = TransformZ(input.pos.z, ZNear, ZFar, Fov, g_ScreenWidth);
    }

    
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

    if(ZBuff_Mode == ZBuff_Persp)
    {
        //#1перспективный z буфер глюки с аптечками и спрайтами
        //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    }
    
    else if(ZBuff_Mode == ZBuff_Linear)
    {
        //#2линейный z буфер нет глюков с аптечками и спрайтами
        output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    }
    
    else if(ZBuff_Mode == ZBuff_TR2)
    {
        //#3взято z буффер из TR2 расчет
        output.pos.z = TransformZ_Sprite(input.pos.z, ZNear, ZFar, Fov, g_ScreenWidth);
    }

    
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
    //float rhw = 1.0f / input.rhw;
    //float2 tex = input.tex * rhw;

    float2 tex = input.tex / input.rhw;

    //float4 col = input.Color * rhw;
    float4 col = input.Color;

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

    if(ZBuff_Mode == ZBuff_Persp)
    {
        //#1перспективный z буфер глюки с аптечками и спрайтами
        //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    }
    
    else if(ZBuff_Mode == ZBuff_Linear)
    {
        //#2линейный z буфер нет глюков с аптечками и спрайтами
        output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    }
    
    else if(ZBuff_Mode == ZBuff_TR2)
    {
        //#3взято z буффер из TR2 расчет
        output.pos.z = TransformZ(input.pos.z, ZNear, ZFar, Fov, g_ScreenWidth);
    }
    
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

    if(ZBuff_Mode == ZBuff_Persp)
    {
        //#1перспективный z буфер глюки с аптечками и спрайтами
        //output.pos.z = (input.pos.z * Q - Q * ZNear) / input.pos.z;
    }
    
    else if(ZBuff_Mode == ZBuff_Linear)
    {
        //#2линейный z буфер нет глюков с аптечками и спрайтами
        output.pos.z = (input.pos.z - ZNear) / (ZFar - ZNear);
    }
    
    else if(ZBuff_Mode == ZBuff_TR2)
    {
        //#3взято z буффер из TR2 расчет
        output.pos.z = TransformZ(input.pos.z, ZNear, ZFar, Fov, g_ScreenWidth);
    }

    
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