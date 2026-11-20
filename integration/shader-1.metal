  } else if(type==49 || type==50){
    s.kind=4; s.alb=type==49?float3(.984,.997,.993):float3(.96,.988,.976);
  } else if(type==51 || type==52){
    float3 q=pl*float3(57.,80.,75.)+float3(2.62,16.2,5.6);
    float wisps=fbm3(q*5.7,3);
    float patch=fbm3(q,3)+.07*(wisps-.45);
    float3 silver=mix(float3(.57,.59,.54),float3(.93,.84,.65),smoothstep(.27,.61,wisps));
    float3 vermilion=mix(float3(.64,.064,.008),float3(.96,.23,.023),smoothstep(.3,.6,wisps));
    float red=smoothstep(type==51?.437:.486,type==51?.489:.529,patch);
    float snout=smoothstep(.068,.093,pl.x);
    float3 col=mix(silver,vermilion,red*(1.-.80*snout));
    float pigment=fbm3(q*3.6+float3(7.,-2.,5.),3);
    float ink=smoothstep(.565,.618,pigment+.078*(wisps-.50))*smoothstep(.15,.50,patch)*(1.-.89*snout);
    col=mix(col,float3(.077,.066,.071),ink*(type==51?.79:.94));
    float belly=smoothstep(-.012,-.031,pl.y);col=mix(col,float3(.79,.73,.60),.34*belly);
    s.alb=col*(.89+.18*vnoise(pl*2300.));s.spec=1.5;s.rough=.26+.084*wisps;s.f0=.052;
  } else if(type==53){
    float stripe=.5+.5*cos(u*83.+.35*sin(v*4.+u*7.));
    float rays=pow(stripe,13.);float fine=pow(.5+.5*cos(u*169.+v*2.),8.);
    float pigment=fbm3(pl*145.+float3(8,0,3),3);
    float soot=smoothstep(.51,.66,pigment)*smoothstep(.97,.24,v);
    float3 neutral=mix(float3(.39,.39,.32),float3(.71,.70,.62),rays+.15*fine);
    s.alb=mix(neutral,float3(.071,.065,.065),.80*soot);s.tr=s.alb*.45;
    s.haze=clamp(.28+.44*v-.44*rays-.19*soot,.06,.78);
    s.spec=.91;s.rough=.28;bump=float2(.033*cos(u*83.+.35*sin(v*4.+u*7.)),0);
  } else if(type==62){
    float fibres=vnoise(float3(cos(2.*PI*u)*24.,sin(2.*PI*u)*24.,v*3.));
    float3 iris=M.a.rgb*(.88+.23*fibres);
    iris=mix(iris,float3(.385,.31,.24),smoothstep(.73,1.,v)*.86);
    s.alb=mix(float3(.039,.032,.028),iris,smoothstep(.45,.56,v));
    s.spec=.83;s.f0=.038;s.rough=M.a.w;
  } else if(type==55){
    float rachis=exp(-abs(u)*70.);
    float marks=.5+.5*cos(82.*v-12.*abs(u)-3.*u*u);
    float vein=pow(marks,11.)*smoothstep(0.,.35,abs(u));
    s.alb=mix(M.a.rgb,float3(.162,.29,.092),.62*smoothstep(.23,.80,marks))*(.91+.17*vnoise(pl*196.));
    s.alb=mix(s.alb,float3(.34,.39,.17),.72*rachis+.25*vein);
    s.tr=s.alb*.37;s.spec=1.15;s.rough=.36;
  } else if(type==54){
    float rib=exp(-abs(u)*25.); s.alb=M.a.rgb*(.86+.20*vnoise(pl*195.)+.23*rib);
    s.tr=s.alb*.37;s.spec=.79;s.rough=.45;
  } else if (type >= 40) {
    float3 base=M.a.rgb, p=pl;
    float tex=vnoise(p*145.f);
    if(type==41){
      float warp=fbm3(float3(p.x*6.,p.y*3.,p.z*1.6),3);
      float ring=sin(p.x*210.+warp*23.+sin(p.z*2.2)*1.1);
      float fiber=vnoise(float3(p.x*250.,p.y*180.,p.z*8.));
      float2 woodUV=fabs(n.y)>.65?p.xz:float2(p.y,p.x+p.z);
      float grain=fbm3(float3(woodUV.x*92.,woodUV.y*4.,3.8),4);
      float pores=smoothstep(.60,.77,vnoise(float3(woodUV.x*620.,woodUV.y*52.,4.)));
      base*=.68+.10*ring+.27*fiber+.27*grain-.19*pores;
      bump3=float3(.085*(grain-.45),0.,.045*(tex-.5));
    } else if(type==42 || type==47 || type==48 || type==43){
      float2 st=fabs(n.y)>.55?p.xz:(fabs(n.z)>.6?p.xy:p.zy);
      float warp=sin(st.x*1800.+2.2*sin(st.y*990.)), weft=sin(st.y*1300.+2.0*cos(st.x*964.));
      if(type==42){base*=.93+.17*fbm3(p*float3(14.,12.,8.),3);}
      if(type==43){
        float2 q=float2(p.x+.02,p.z+.80);
        float border=min(1.32-abs(q.x),1.05-abs(q.y));
        float3 navy=float3(.223,.221,.188), straw=float3(.48,.394,.278),rust=float3(.329,.239,.17);
        float2 tile=abs(fract((q+float2(.17,.08))/float2(.48,.39))-.5)*2.;
        float diamond=tile.x+tile.y;
        base=rust;
        if(diamond<.82)base=straw;if(diamond<.68)base=navy;if(diamond<.43)base=rust;if(diamond<.19)base=straw;
        if(border<.125){base=navy;if(fract(border*36.)<.22)base=straw;}
      }
      if(type==48){
        float2 knit=st*float2(560.,430.);
        float loop=sin(knit.x+1.05*sin(knit.y))*cos(knit.y);
        base*=.81+.16*loop+.21*vnoise(float3(st*390.,6.));
        float band=abs(p.z+2.09); if(band<.013 || (band>.030&&band<.034))base*=.69;
      }
      base*=.87+.14*warp+.095*weft+.18*tex;
      bump3=float3(warp*.047,weft*.04,0.);
    } else if(type==45){
      float x=p.x<0.?(p.x+.46)/.541+.5:(p.x-.35)/.541+.5, y=(p.y-1.65)/.704+.5;
      float shift=p.x<0.?0.:.18;
      base=float3(.79,.67,.49);
      if(length((float2(x,y)-float2(.66-shift,.72))*float2(1.,1.30))<.14)base=float3(.76,.26,.11);
      if(y<.42+sin(x*4.+shift*12.)*.16)base=float3(.31,.40,.32);
      if(y<.30+sin(x*5.+2.+shift*9.)*.10)base=float3(.125,.22,.21);
      if(y<.15+sin(x*7.+shift*5.)*.105)base=float3(.42,.23,.13);
      base*=.94+.09*tex;
    } else if(type==44){
      float band=sin(p.z*182.+3.*sin(p.x*48.)+4.1*p.y);
      if(base.r>.5 && base.g<.4)base*=.70+.29*smoothstep(-.7,.7,band);
      base*=.78+.40*vnoise(float3(p.x*2300.,p.y*1700.,p.z*470.));
    } else if(type==40 && M.a.r>.70 && M.a.g>.60 && (abs(p.z)>3.388 || abs(p.x)>2.79) && p.y<.74 && p.y>.09){
      base=float3(.48,.486,.423)*(.935+.13*fbm3(p*22.,3));
    } else {base*=.91+.13*tex+.07*vnoise(p*3.);}
    s.alb=base; s.spec=.45;
