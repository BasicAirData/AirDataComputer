//Calculates cp/cv of moist air
//Dry air heat ratio correct from (200 , 450)°K  (exp unc < 0.1%)
//Moist air heat ratio correct from (253 , 323)°K
exec('rhoair.sci')
function [heatratio]=cpcvair(T,p,RH)
    A=6.116441;
    m=7.591386
    Tn=240.7263
    Tc=T-273.15;
    pws= A*10^(m*Tc/(Tc+Tn))//hPa Water saturation pressure (-20 to 50 °C)
    pw=pws*RH*100//Pa
    massair=rhoairID(T, p,RH);
    if (RH>0)
        x=2.16679*pw/(T)/1000/massair;
    else
        x=0;
    end
    cp=1002.5+275e-6*(T-200)^2+x*1000*(1.856+2e-4*T)//Specific thermal capacity at constant pressure J/(kg°K)
    cv=717.8 + 0.07075*(T-300)+0.26125e-3*(T-300)^2+x*1000*(1.856+2e-4*T)//Specific thermal capacity at constant volume J/(kg°K))
    heatratio=cp/cv;
endfunction
