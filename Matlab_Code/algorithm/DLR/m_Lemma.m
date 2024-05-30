% function Y = m_Lemma(X, thr)
% [U,S,V] = svd(X, "econ");
% Vt = V.';
% [n1,n2] = size(X);
% n = min(n1,n2);
% for i = 1:n
%     S(i,i) = max(S(i,i) - thr, 0);
% end
% Y = U * S * Vt;
% end

function Y = m_Lemma(X, thr)
[U,sigma,V] = svd(X,'econ');

sigma = diag(sigma);
svp = min(length(find(sigma>thr)),10);

 Y = U(:, 1:svp) * diag(sigma(1:svp) - thr) * V(:, 1:svp)'; 