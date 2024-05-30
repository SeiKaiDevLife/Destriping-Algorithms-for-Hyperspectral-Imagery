function q = Cal_q(D2, x, b2, Mu, Nu)
function X=SoftTh(B,lambda)
      
       X=sign(B).*max(0,abs(B)-(lambda/2));
       
end
q=SoftTh(D2*x+b2,Mu/Nu);
end