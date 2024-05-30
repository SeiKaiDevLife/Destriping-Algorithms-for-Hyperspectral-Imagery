function Result = m_NuclearNorm(X)
Xt = X.';
XtX = (Xt*X)^(1/2);
Result = trace(XtX);
end
