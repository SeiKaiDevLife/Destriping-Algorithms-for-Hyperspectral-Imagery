function D = Cal_D(dim)

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

D=opTV1(dim);
D=D';
end