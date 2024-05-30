function Tk = t_TSub(C)
SizeD = size(C);
h               = SizeD(1);
w               = SizeD(2);
d               = SizeD(3);

Eny_y   = ( abs(psf2otf([+1; -1], [h,w,d])) ).^2  ;

determ  =  Eny_y;

Tk      = real( ifftn( fftn(C) ./ (determ + 1) ) );