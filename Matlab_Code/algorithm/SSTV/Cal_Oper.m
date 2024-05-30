function [D1,D2] = Cal_Oper(m, n, dim)

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

function opD=opTV1(m)
%Make two vectors of elements -1 and 1 of lengths (m-1)
B=[ -1*ones(m-1,1),ones(m-1,1)]; 
%Make sparse diagonal matrix D of size m-1 by m
%with -1's on zeroth diagonal and 1's on 1st diagonal;
D=spdiags(B,[0,1],m-1,m);
tempD = zeros(1,m);
tempD(1,m) = -1;
opD=[D;tempD];
end

% Make total variation matrix
Dh=TVmatrix(m,n,'H');
Dv=TVmatrix(m,n,'V');
D=opTV1(dim);D=D';
D1=kron(D',Dh); 
D2=kron(D',Dv);

end
