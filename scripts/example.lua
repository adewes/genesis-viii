
p=pop.new();
--dir="data/distri_dax";
--dir="data/terror";
--dir="data/es_morgan_stanley_ts=10";
--dir="data/es_morgan_stanley_comp";
--dir="data/var_amex_new";
--dir="data/es_amex_wt";
dir="data/copula_test_new";
--This is the copula we used in this sample:
--mix(0.5,STAT::normal_copula(u,v,-0.7),mix(0.4,STAT::gumbel_copula(u,v,4.0),STAT::clayton_copula(u,v,2.0)));
--dir="data/copula_test_mixture";
--dir="data/var_msft_hurst";
--dir="data/distri";
pop.output_dir(p,dir);
logger.reopen(dir);
print("Mutation rate:",pop.mutation_rate(p,0.05),"\n");
print("Shrink rate:",pop.shrink_rate(p,0.025),"\n");
print("Crossover size:",pop.crossover_size(p,0.2),"\n");
print("Tournament size:",pop.tournament_size(p,6),"\n");
--pop.create(p,1000,10,program.copula);
pop.create(p,1000,10,program.copula,"data/stocks/dow.csv","data/stocks/dax.csv",6,",");
--pop.create(p,1000,10,program.terror);
--pop.create(p,10000,10,program.es,"data/var_amex_good/best_60.tsv");
--pop.create(p,1000,10,program.var);
--pop.create(p,500,10,program.var,"data/stocks/amex.csv",6,",");
--pop.create(p,1000,10,program.var);
--pop.create(p,10000,10,program.distribution,"data/stocks/dax.csv",6,",",1);
pop.run(p,50);
exit(0);

copula=Matrix();
copula_real=Matrix();
chi=Matrix();
--copula:load("data/copula_test_mixture/copula_30.tsv");
copula_real:load("data/copula_test_mixture/copula_density.tsv");
incr=101;
sum=0;
for i=0,90,10 do
for j=0,90,10 do
	x1=copula_real:get(i*incr+j,0);
	y1=copula_real:get(i*incr+j,1);
	c1=copula_real:get(i*incr+j,2);
	x2=copula_real:get((i+10)*incr+j,0);
	y2=copula_real:get((i+10)*incr+j,1);
	c2=copula_real:get((i+10)*incr+j,2);
	x3=copula_real:get(i*incr+j+10,0);
	y3=copula_real:get(i*incr+j+10,1);
	c3=copula_real:get(i*incr+j+10,2);
	x4=copula_real:get((i+10)*incr+j+10,0);
	y4=copula_real:get((i+10)*incr+j+10,1);
	c4=copula_real:get((i+10)*incr+j+10,2);
--	print("1:",x1,",",y1,", 2:",x2,",",y2,", 3:",x3,",",y3,", 4:",x4,",",y4,"\n");
	p=c1+c4-c2-c3;
	if p>0 then
		sum=sum+p;
	end
	print("p(",x1,",",y1,")=",p,"\n");
end
end
print("sum=",sum,"\n");
exit(0);