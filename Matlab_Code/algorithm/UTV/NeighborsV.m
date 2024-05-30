function Result = NeighborsV(V)
[hV,wV] = size(V);
Result = zeros(hV,wV);

for i = 1:hV
    for j = 1:wV
        i_up = min(hV, i + 1);
        j_up = j;
        
        i_down = max(1,i - 1);
        j_down = j;
        
        Result(i,j) = V(i, j) * 2 - V(i_up,j_up) - V(i_down,j_down);
    end
end
end
