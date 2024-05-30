function Dh=Cal_Dh(m,n)
D = spdiags([-ones(n,1) ones(n,1)],[0 1],n,n);
D(n,:) = 0;
D = kron(D,speye(m));
Dh=D;
end