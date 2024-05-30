function Xk = t_XSub(C)
SizeD = size(C);
h               = SizeD(1);
w               = SizeD(2);
d               = SizeD(3);

Eny_x   = ( abs(psf2otf([+1, -1], [h,w,d])) ).^2  ;
Eny_z   = ( abs(psf2otf([+1, -1], [w,d,h])) ).^2  ;
Eny_z   =  permute(Eny_z, [3, 1 2]);

determ  =  Eny_x + Eny_z;

Xk      = real( ifftn( fftn(C) ./ (determ + 4) ) );