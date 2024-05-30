function Y = t_Lemma(X, thr)
[U,S,V] = t_SVD(X);
Vt = t_ConTran(V);
[n1,n2,n3] = size(X);
n = min(n1,n2);
for i = 1:n3
    for j = 1:n
        S(j,j,i) = max(S(j,j,i) - thr, 0);
    end
end
US = t_Product(U,S);

Y = t_Product(US,Vt);
end

function [U,S,V] = t_SVD(X)

D = fft(X, [], 3);
[n1,n2,n3] = size(X);
U = zeros(n1,n1,n3);
S = zeros(n1,n2,n3);
V = zeros(n2,n2,n3);

for i = 1 : n3
    [Ui,Si,Vi] = svd(D(:,:,i));
    U(:,:,i) = Ui;
    S(:,:,i) = Si;
    V(:,:,i) = Vi;
end
U = ifft(U,[],3);
S = ifft(S,[],3);
V = ifft(V,[],3);
end

function Xt=t_ConTran(X)
[n1,n2,n3] = size(X);
Xt = zeros(n2,n1,n3);
Xt(:,:,1) = X(:,:,1)';
for i = 2 : n3
    Xt(:,:,i) = X(:,:,n3-i+2)';
end
end

function C = t_Product(A,B)

[n1,n2,n3] = size(A);
[m1,m2,m3] = size(B);

if n2 ~= m1 || n3 ~= m3 
    error('Inner tensor dimensions must agree.');
end

A = fft(A,[],3);
B = fft(B,[],3);
C = zeros(n1,m2,n3);

halfn3 = ceil((n3+1)/2);
for i = 1 : halfn3
    C(:,:,i) = A(:,:,i)*B(:,:,i);
end
for i = halfn3+1 : n3
    C(:,:,i) = conj(C(:,:,n3+2-i));
end
C = ifft(C,[],3);
end