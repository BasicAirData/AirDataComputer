//Calculate viscosity. Sutherland's formula
function [viscosity]=viscosityair(T)
viscosity = 18.27*(291.15+120)/(T+120)*((T/291.15)^(3/2))*1e-6; 
endfunction
