N=10000
step=5
beginQuery="SELECT asText(DouglasPeuckerSimplify(tgeompoint "
", sqrt(3)))"
Lambda=(0.01,0.12,0.25,0.5,0.65,0.77,1) #1% 12% 25% 50% 65% 77% 100%
for i in range(1,N,step):
    for j in Lambda :
        points = "'[Point(1 1 1)@2000-01-01," * i
        points = points[:-1] + "]'"
        query = beginQuery + points + ", " + str(j) + "))"
        print(query)
        input()