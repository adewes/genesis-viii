#include "terrorism_program.h"
#include "../utils/statistics.h"
#include "../utils/nr/nr.h"
#include "../gp/leaf.h"

Matrix TerrorismProgram::values,TerrorismProgram::categories,TerrorismProgram::cleaned;
float TerrorismProgram::latitude,TerrorismProgram::longitude,TerrorismProgram::date,TerrorismProgram::country,TerrorismProgram::region,TerrorismProgram::suicide_attack,TerrorismProgram::attack_type,TerrorismProgram::target_type,TerrorismProgram::target_nationality,TerrorismProgram::perp_gname,TerrorismProgram::num_perps,TerrorismProgram::weapon_type,TerrorismProgram::num_fatalities,TerrorismProgram::num_injured,TerrorismProgram::damage,TerrorismProgram::successful;

TerrorismProgram::TerrorismProgram() : Program()
{
}

TerrorismProgram::~TerrorismProgram()
{

}

#define LATITUDE 14
#define LONGITUDE 15
#define DATE 3
#define COUNTRY 9
#define REGION 10
#define SUICIDE_ATTACK 32
#define ATTACK_TYPE 33
#define TARGET_TYPE 40
#define TARGET_NATIONALITY 45
#define PERP_GNAME 57
#define NUM_PERPS 75
#define WEAPON_TYPE 90
#define NUM_FATALITIES 100
#define NUM_INJURED 103
#define DAMAGE 109
#define SUCCESSFUL 31
float TerrorismProgram::Fitness()
{
	unsigned int i=0;
	float f,fitness;
	unsigned int cnt=0;
	fitness=0.0;
	for(i=0;i<cleaned.Dim() && cnt<500;i++)
	{
		latitude=cleaned[i][LATITUDE];
		longitude=cleaned[i][LONGITUDE];
		date=cleaned[i][DATE];
		country=cleaned[i][COUNTRY];
		region=cleaned[i][REGION];
		suicide_attack=cleaned[i][SUICIDE_ATTACK];
		attack_type=cleaned[i][ATTACK_TYPE];
		target_type=cleaned[i][TARGET_TYPE];
		target_nationality=cleaned[i][TARGET_NATIONALITY];
		perp_gname=cleaned[i][PERP_GNAME];
		num_perps=cleaned[i][NUM_PERPS];
		weapon_type=cleaned[i][WEAPON_TYPE];
		num_fatalities=cleaned[i][NUM_FATALITIES];
		num_injured=cleaned[i][NUM_INJURED];
		damage=cleaned[i][DAMAGE];
/*		cout << "Suicide attack = " << suicide_attack << "\n";
		cout << "Region = " << region << "\n";
		cout << "Country = " << country << "\n";
		cout << "Weapon type = " << weapon_type << "\n";
		cout << "Fatalities " << num_fatalities << "\n";
		cout << "Injured " << num_injured << "\n";*/
		successful=cleaned[i][SUCCESSFUL];
		if (num_injured>=0 && num_fatalities>=0)
		{
			cnt++;
			f=Eval();
			fitness+=pow(f-num_injured,2.f);
		}
	}
	return -log(1.0+fitness);
}

void TerrorismProgram::Write(std::string directory,unsigned int generation)
{
	
}

void TerrorismProgram::Setup(std::string directory,vector<std::string> parameters)
{
	static bool Initialized=false;
	if (Initialized==true)
		return;
	cout << "Setting up terrorism program...\n";
	Initialized=true;
	unsigned int i,j,k;
	unsigned int found=-1;
	char str[1024];
	i=0;
	TerminalManager *TM=TM->i();
	Matrix terror,fatalities,distri,categorized,cleaned_categories;
/*	terror.Load("data/terror.tsv","\t",NULL);
	vector <bool> is_float;
	is_float.resize(terror[1].Dim());
	for(i=0;i<terror[1].Dim();i++)
	{
		is_float[i]=1;
		if (!terror[1][i].IsFloat())
		{
			is_float[i]=0;
			cout << "String variable:" << (std::string)terror[0][i] << "!\n";
			for(j=1;j<terror.Dim();j++)
			{
				found=-1;
				for(k=0;k<categories[i].Dim();k++)
				{
					if ((std::string)categories[i][k]==(std::string)terror[j][i])
					{
						found=k;
						break;
					}
				}
				if (found==-1)
				{
					cout << "New category[" << j << "]:" << (std::string)terror[j][i] << "\n";
					found=categories[i].Dim();
					categories.Set(i,found,terror[j][i]);
				}
				if(categories[i].Dim()>510)
					break;
				terror.Set(j,i,found);
			}
		}
	}
	cout << "Starting data transfer...\n";
	unsigned int row=0;
	for(i=0;i<terror[1].Dim();i++)
	{
		if (is_float[i]==0)
		{
			cout << "Adding categorical column " << (std::string)terror[0][i] << "\n";
			if (categories[i].Dim()<500)
			{
				for(j=0;j<terror.Dim();j++)
					cleaned.Set(j,row,terror[j][i]);
				sprintf(str,"%u_",row);
				std::string hint=str;
				hint.append((std::string)(terror[0][i]));
				cleaned_categories.Set(row,0,hint);
				for(j=0;j<categories[i].Dim();j++)
					cleaned_categories.Set(row,j+1,categories[i][j]);
				row++;
			}
		}
		else
		{
			for(j=0;j<terror.Dim();j++)
				cleaned.Set(j,row,terror[j][i]);
			sprintf(str,"%u_",row);
			std::string hint=str;
			hint.append((std::string)(terror[0][i]));
			cleaned_categories.Set(row,0,hint);
			row++;
		}	
	}
	cleaned.Write("data/cleaned.tsv");
	cleaned_categories.Write("data/categories.tsv");
	exit(0);*//*
	cleaned.Load("data/cleaned.tsv");
	std::vector <float> mu_vec,sigma_vec;
	mu_vec.resize(cleaned[0].Dim());
	sigma_vec.resize(cleaned[0].Dim());
	Matrix correlations,correlations_corrected;
	correlations.Load("data/correlations.tsv");
	for(i=0;i<correlations.Dim();i++)
		for(j=0;j<i;j++)
		{
			if (i==j)
				correlations_corrected.Set(i,j,0.0);
			else if ((float)correlations[i][j]==-2.f)
				correlations_corrected.Set(i,j,0.f);
			else
				correlations_corrected.Set(i,j,(float)correlations[i][j]);
		}
	correlations_corrected.Write("data/correlations_cor.tsv");
	exit(0);
	float mu,sigma;
	for(i=0;i<cleaned[0].Dim();i++)
	{
		mu=0.0;
		for(j=1;j<cleaned.Dim();j++)
		{
			mu+=(float)cleaned[j][i];
		}
		mu/=cleaned.Dim()-1;
		cout << "E(" << i << ") = " << mu << "\n";
		mu_vec[i]=mu;
	}	
	for(i=0;i<cleaned[0].Dim();i++)
	{
		sigma=0.0;
		for(j=1;j<cleaned.Dim();j++)
		{
			sigma+=pow((float)cleaned[j][i]-mu_vec[i],2.f);
		}
		sigma/=cleaned.Dim()-2;
		cout << "Var(" << i << ") = " << sqrt(sigma) << "\n";
		sigma_vec[i]=sqrt(sigma);
	}	
	float correlation=0.0;
	for(i=0;i<cleaned[0].Dim();i++)
		for(j=0;j<cleaned[0].Dim();j++)
		{
			correlation=0.0;
			for(k=1;k<cleaned.Dim();k++)
				correlation+=((float)cleaned[k][i]-mu_vec[i])*((float)cleaned[k][j]-mu_vec[j])/sigma_vec[i]/sigma_vec[j];
			correlation/=cleaned.Dim()-1;
			if (correlation!=correlation)
				correlation=-2.;
			cout << "corr(" << i << "," << j << ") = " << correlation << "\n";
			correlations.Set(i,j,correlation);
		}
	correlations.Write("data/correlations.tsv");*/
/*	for(i=0;i<terror[0].Dim();i++)
	{
		sprintf(str,"_%d",i);
		std::string my_str=terror[0][i];
		my_str.append(str);
		cout << my_str << "\t";
		terror.Set(0,i,my_str);
//		terror[0][i]=my_str;
		cout << "\n";
	}
*//*	unsigned int cnt=0;
	for(i=1;i<terror.Dim();i++)
	{
		if ((float)terror[i][102]!=-99 && (float)terror[i][105]!=-9 && (float)terror[i][105]!=-99)
		{
			fatalities.Set(cnt,0,terror[i][0]);
			fatalities.Set(cnt,1,terror[i][102]);//Fatalities
			fatalities.Set(cnt,2,terror[i][105]);//Injured
			fatalities.Set(cnt,3,terror[i][2]);//Timestring
			fatalities.Set(cnt,4,terror[i][3]);//Year
			fatalities.Set(cnt,5,terror[i][4]);//Month
			fatalities.Set(cnt,6,terror[i][5]);//Day
			cnt++;
		}
	}
	Matrix fatalities_histo,injured_histo;
	fatalities_histo=STAT::generate_histogram(fatalities,1,200);
	injured_histo=STAT::generate_histogram(fatalities,2,200);
	fatalities_histo.Write("data/fatalities_histo.tsv","\t",NULL);
	injured_histo.Write("data/injured_histo.tsv","\t",NULL);
	fatalities.Write("data/fatalities_injured.tsv","\t",NULL);
	terror.Write("data/terror_mod.tsv","\t",NULL);*/
	TM->Clear();
	TM->Add("latitude",&latitude);
	TM->Add("longitude",&longitude);	
	TM->Add("date",&date);
	TM->Add("country",&country);
	TM->Add("region",&region);
	TM->Add("suicide_attack",&suicide_attack);
	TM->Add("attack_type",&attack_type);
	TM->Add("target_type",&target_type);
	TM->Add("target_nationality",&target_nationality);
	TM->Add("perp_gname",&perp_gname);
	TM->Add("num_perps",&num_perps);
	TM->Add("weapon_type",&weapon_type);
/*	TM->Add("num_fatalities",&num_fatalities);
	TM->Add("num_injured",&num_injured);*/
	TM->Add("damage",&damage);
	TM->Add("successful",&successful);
	cleaned.Load("data/cleaned.tsv");
	categories.Load("data/categories.tsv");
}
