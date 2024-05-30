function p = Cal_p(D1, x, b1, Mu, Nu)
function X=SoftTh(B,lambda)
      
       X=sign(B).*max(0,abs(B)-(lambda/2));
       
end
p=SoftTh(D1*x+b1,Mu/Nu);
end