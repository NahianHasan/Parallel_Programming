clear all
close all
clc


N=36;P=4;
EO = @(i,P,N) (floor((P*(i+1)-1)/N));
for ix=0:P-1
    LB=floor(ix*N/P);
    UB=floor((ix+1)*N/P)-1;
    disp([num2str(LB),'-',num2str(UB),'--',num2str(UB-LB+1)])
end

EO(6,P,N)%ID of process controlling data i

%%%Round Robin Distribution
disp('Round Robin')
B=1;
step_size=P;
for ix=0:P-1
    ids=ix:step_size:N-1;
    disp(ids)
end


%%%Block Cyclic Distribution
disp('Block Cyclic Distribution')
B=3;%B=1 refers to Round-Robin Distribution
for ix=0:P-1
    b=0;
    I=[];
    ids=b*B*P+ix*B:b*B*ix+(ix+1)*B-1;
    while ids(end)<N
        I=[I,ids];
        b=b+1;
        ids=b*B*P+ix*B:b*B*P+(ix+1)*B-1;
    end
    disp(['Process = ',num2str(ix),'---',num2str(I)])
end