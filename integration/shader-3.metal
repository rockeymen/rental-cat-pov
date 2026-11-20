    if(P.x>1.02&&P.x<2.073&&P.y>.348&&P.y<.845&&P.z>-1.070&&P.z<-.63){
      float3 ld=normalize(float3(clamp(P.x,1.15,1.93)-P.x,.884-P.y,-.855-P.z));
      float2 cp=P.xz+float2(P.y*.5,P.y*.24);
      float wave=sin(cp.x*68.+2.5*sin(cp.y*34.))+sin(cp.y*63.+1.8*sin(cp.x*39.));
      float caustic=pow(max(0.,1.-abs(wave)*2.),4.);
      float surface=.88+.45*caustic;
      float3 hLED=normalize(ld+V);
      L+=thr*float3(.93,.95,.87)*pow(max(dot(hLED,s.n),0.),29.)*s.spec*.27;
      L+=thr*s.alb*float3(.90,1.02,1.05)*(.12+.86*max(dot(s.n,ld),0.))*surface;
    }
    // Lamp bleeds warm light onto the adjacent wall, pillows and cabinet.
    float3 lamp=float3(-1.52,1.31,-2.66),lv=lamp-P;float dl=length(lv);lv/=max(dl,.01);
    if(dl<2.5 && dl>.205){float nd=max(0.,dot(s.n,lv));
      if(nd>.001){float3 vis=shadow(S,P+s.ng*eps,lv,max(.001,dl-.205),ray.time,rs,U);
        L+=thr*s.alb*float3(.37,.215,.089)*vis*nd/(dl*dl+.18);}}
    // Sample the open sash as an area source; this carries diffuse skylight into the flat.
    float3 wp=float3(-2.79,.78+1.66*rnd(rs),-2.26+3.78*rnd(rs));
    float3 toW=wp-P; float dw=length(toW);float3 wi=toW/max(dw,.001);
    float wf=max(dot(s.n,wi),0.)*max(-wi.x,0.)*6.27/max(dw*dw,.16);
    if(wf>.001){float3 occ=shadow(S,P+s.ng*eps,wi,max(dw-.015,.002),ray.time,rs,U);
      L+=thr*s.alb*float3(1.06,1.10,1.13)*min(wf,3.)*occ;}
