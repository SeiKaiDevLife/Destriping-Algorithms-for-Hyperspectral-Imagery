function opD=TVmatrix(m,n,str)

if str=='H' % This will give matrix for Horizontal Gradient
    D = spdiags([-ones(n,1) ones(n,1)],[0 1],n,n);
    D(n,:) = 0;
    D = kron(D,speye(m));
   
elseif str=='V' %This will give matrix for Verticle Gradient
   D = spdiags([-ones(m,1) ones(m,1)],[0 1],m,m);
   D(m,:) = 0;
   D = kron(speye(n),D);
end
opD=D;
end