function Xk = FFT(C, dAlpha, dBeta, dGamma)
SizeD = size(C);
h               = SizeD(1);
w               = SizeD(2);
d               = SizeD(3);

Eny_x   = ( abs(psf2otf([+1; -1], [h,w,d])) ).^2  ;
Eny_y   = ( abs(psf2otf([+1, -1], [h,w,d])) ).^2  ;
Eny_z   = ( abs(psf2otf([+1, -1], [w,d,h])) ).^2  ;
Eny_z   =  permute(Eny_z, [3, 1 2]);

determ  =  1 + dAlpha * Eny_x + dBeta * Eny_y + dGamma * Eny_z;

Xk      = real( ifftn( fftn(C) ./ determ ) );

end
