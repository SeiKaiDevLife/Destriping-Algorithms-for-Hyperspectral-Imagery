function [U,S,V] = tsvd(X)

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
