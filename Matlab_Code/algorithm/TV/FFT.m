function Xk = FFT(mu, lambda, f, dx, bx, dy, by)
SizeD = size(f);
h               = SizeD(1);
w               = SizeD(2);

Eny_x   = ( abs(psf2otf([+1; -1], [h,w])) ).^2  ;
Eny_y   = ( abs(psf2otf([+1, -1], [h,w])) ).^2  ;

determ  =  Eny_x + Eny_y;

[~,Dt]      = defDDT();

[Datax, ~] = Dt(dx - bx);
[~, Datay] = Dt(dy - by);

C = mu * f + lambda * (Datax + Datay);

Xk      = real( ifftn( fftn(C) ./ (lambda * determ + mu) ) );

end
