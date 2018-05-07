classdef plotspec
    %PLOTSPEC plot spec of each simulator and solvers
    %   plot line marker, color etc.
    
    properties (Constant)
        RAIRAI =                {'-d', 'Rai',           [137, 165, 78] ./ 255};
        BULLETSEQUENCEIMPULSE = {'-s', 'BtSeqImp',     [170, 70, 67] ./ 255};
        BULLETNNCG =            {'-*', 'BtNNCG',       [170, 70, 67] ./ 255};
        BULLETMLCPDANTZIG =     {'-o', 'BtDantzig',    [170, 70, 67] ./ 255};
        BULLETMLCPLEMKE =       {'-p', 'BtLemke',      [170, 70, 67] ./ 255};
        BULLETMLCPPGS =         {'-h', 'BtPGS',        [170, 70, 67] ./ 255};
        BULLETMULTIBODY =       {'-d', 'BtMultibody',  [170, 70, 67] ./ 255};
        ODESTANDARD =           {'-o', 'OdeStd',       [219, 132, 61] ./ 255};
        ODEQUICK =              {'-*', 'OdeQuick',     [219, 132, 61] ./ 255};
        MUJOCOPGS =             {'-s', 'MjcPGS',       [69, 114, 167] ./ 255};
        MUJOCOCG =              {'-*', 'MjcCG',        [69, 114, 167] ./ 255};
        MUJOCONEWTON =          {'-o', 'MjcNewton',    [69, 114, 167] ./ 255};
        DARTDANTZIG =           {'-o', 'DartDantzig',  [128, 105, 155] ./ 255};
        DARTPGS =               {'-*', 'DartPGS',      [128, 105, 155] ./ 255};
    end
    
%     methods
%         function obj = plotspec(inputArg1,inputArg2)
%             %PLOTSPEC Construct an instance of this class
%             %   Detailed explanation goes here
%             obj.Property1 = inputArg1 + inputArg2;
%         end
%         
%         function outputArg = method1(obj,inputArg)
%             %METHOD1 Summary of this method goes here
%             %   Detailed explanation goes here
%             outputArg = obj.Property1 + inputArg;
%         end
%     end
end

