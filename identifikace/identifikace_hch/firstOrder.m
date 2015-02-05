function [ outputVektor ] = firstOrder( inputVektor, dt, P )

    outputVektor(1, 1) = 0;
    
    for i = 2:1:length(inputVektor)
        outputVektor(i, 1) = outputVektor(i-1)*P(1) + inputVektor(i-1)*P(2)*dt(i);
    end

end

