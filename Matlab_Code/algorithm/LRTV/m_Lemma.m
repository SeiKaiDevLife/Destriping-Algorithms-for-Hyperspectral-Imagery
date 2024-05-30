function Y = m_Lemma(X, thr)
%SINGULAR_VALUE_SHRINKAGE Summary of this function goes here
%   Detailed explanation goes here
% tic;
[U,S,V]=svd(X,'econ');
% [U,S,V]=lansvd(X);
S = diag(S);
svp = length(find(S > thr));
% fprintf(1,'svp:%d\n',svp);
S = S(1:svp)-thr;
% S=diag(wthresh(diag(S),'s',tau)); % 在实现里它只考虑>tau的情况
U = U(:, 1:svp);
V = V(:, 1:svp);
Y = U*diag(S)*V';
% t=toc;
% fprintf(1,'svd shrinkage takes:%f\n',t);
end