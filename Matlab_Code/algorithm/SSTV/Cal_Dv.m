function Dv=Cal_Dv(m,n)
D = spdiags([-ones(m,1) ones(m,1)],[0 1],m,m);
D(m,:) = 0;
D = kron(speye(n),D);
Dv=D;
end