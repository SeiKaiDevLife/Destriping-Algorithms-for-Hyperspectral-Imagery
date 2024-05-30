function Output = SSTV(x, y, Nu, s, p, q, b1, b2, D1, D2, m, n, dim)

    function y = afun(x,str)
       tempval= Nu*((D1'*(D1*x))+(D2'*(D2*x)))+ x;
            switch str
                case 'transp'
                    y = tempval;
                case 'notransp'
                    y = tempval;
            end
    end

bigY=(y-s)+Nu*D1'*(p-b1)+Nu*D2'*(q-b2);
[x,~]=lsqr(@afun,bigY,1e-15,20,[],[],x); 

Output=reshape(x,m,n,dim);
end