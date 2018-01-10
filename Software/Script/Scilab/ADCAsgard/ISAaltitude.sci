function [h]=ISAaltitude(Ps)
        Ps=Ps*0.000295299875080277;//Pa to inHg Conversion
        //Using Goodrich 4081 Air data handbook formula
        h=(29.92126^0.190255 - Ps^0.190255)*76189.2339431570; //US atmosphere 1962
        //Back to SI
        h=h*0.3048;
        //76189.2339431570*(_p*0.000295299875080277)^0.190255  
        //Unceratinty
    //    _uh=4418.19264813511*pow(Ps,-0.809745)*_up*0.000295299875080277;
endfunction
