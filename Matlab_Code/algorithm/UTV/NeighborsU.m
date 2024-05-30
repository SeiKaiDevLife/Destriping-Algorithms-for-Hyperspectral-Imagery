function Result = NeighborsU(U,lambda)


[hU,wU] = size(U);
Result = zeros(hU,wU);
for i = 1:hU
    for j = 1:wU
        i_left = i;
        j_left = max(1, j - 1);
        
        i_right = i;
        j_right = min(wU, j + 1);
        
        i_up = min(hU, i + 1);
        j_up = j;
        
        i_down = max(1,i - 1);
        j_down = j;
        
        Result(i,j) = lambda * U(i_left, j_left) + lambda * U(i_right, j_right) + U(i_up,j_up) + U(i_down,j_down);
    end
end
end

