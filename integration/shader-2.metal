    if(s.kind==0 && s.haze>0. && rnd(rs)<s.haze){skip=0u;ray.o=P-s.ng*eps;thr*=float3(.996,.985,.96);continue;}
    if(s.kind==4){
      skip=0u;
      float cosi=max(dot(V,s.n),0.0f);
      float fres=.026f+.974f*pow(1.-cosi,5.);
      if(rnd(rs)<fres){ray.d=reflect(ray.d,s.n);ray.o=P+s.ng*eps;}
      else{
        bool face=P.z>-.639&&P.z<-.631 && abs(s.ng.z)>.85 && P.y<.822;
        if(face){float eta=ray.d.z<0.?1./1.333:1.333;float3 rd=refract(ray.d,s.n,eta);
          if(dot(rd,rd)>.1)ray.d=rd;else{ray.d=reflect(ray.d,s.n);ray.o=P+s.ng*eps;continue;}}
        thr*=s.alb;ray.o=P-s.ng*eps;}
      continue;
    }
