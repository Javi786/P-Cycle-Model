class WindErosion: public Module {
public:

    WindErosion(string name): Module(name) { init(); }

    double beta, satur, A0, M0, rho, moved_Oc_vol;
    bool uniform_growth;
    int p, co, oc, atm; //Phosphorus in continental crust, oceans and atmosphere
    
        void init(void) {
        this->links.push_back("CCrust2 -> Atmosphere0");
        this->links.push_back("CCrust2 -> Oceans2");
        this->links.push_back("Oceans2 -> Atmosphere0");
        this->isBidirectional = true;
        this->numOutputs = 3;
        this->init_fluxes(2);

        beta = config->data["WindErosion"]["beta"].as<double>(); //Rate of continental wind erosion - kg.m⁻².yr⁻¹ (current day ~ 0.014 kg.m⁻².yr⁻¹)
        
        uniform_growth = config->data["Erosion"]["uniform_growth"].as<bool>();
        
        satur = config->data["WindErosion"]["satur"].as<double>(); // Saturation mass of P in the Atmosphere
        
        moved_Oc_vol = config->data["WindErosion"]["moved_Oc_vol"].as<double>(); // Ocean Volume of water moved into the atmosphere as sea spray per year (in liters) 
        
        A0 = 1.48e14; // m2, today
        M0 = 2.28e22; // kg, today
        rho = 2700; // kg/m3

        p = s->element_map["p"];

        co = s->reservoir_map["CCrust"];
        oc = s->reservoir_map["Oceans"];
        atm = s->reservoir_map["Atmosphere"];
    }

    void evolve(void) {
        double relative_area;
        if (s->time < 1.5e9) { 
            relative_area = 0.4*(0.02 + 0.64*s->time/1.5e9);
        } else {
            relative_area = 0.4*(0.66 + 0.34*(s->time-1.5e9)/3e9);
        }
        if (uniform_growth) { relative_area = 0.4 - 2.613e-3*exp(- s->time/0.15e9);  } //Parametrization of Emerged land growth

        //double concentration = s->world[co]->masses[p]/(relative_area*M0); //Mass concentration of P in the continents
        double flux = beta*A0*relative_area; //wind erosion per year
	
	double Ocean_P_mass_conc = s->world[oc]->masses[p]/(config->data["Precipitation"]["VOcean"].as<double>()); //Mass Concentration of P in the oceans, kg/liter
	
	double sea_spray_P = moved_Oc_vol*Ocean_P_mass_conc;
	
	double outflux = 0;
	
	double atm_flux = flux + sea_spray_P; 
	
	if (s->world[atm]->masses[p] + flux*s->timestep > satur)
	{
		outflux = (s->world[atm]->masses[p] + (flux + sea_spray_P)*s->timestep - satur)/s->timestep; // If this flux leads to an oversaturated atmosphere, that outflux is authomatically redirected to the Oceans, in kg/yr
		
		atm_flux = flux + sea_spray_P - outflux; //the net flux into the atmosphere, in kg/yr
		
		//if(atm_flux < 0 && abs(atm_flux) > s->world[atm]->fluxes[p])
		//{
		//	atm_flux = - s->world[atm]->fluxes[p];
		//}
	
	}
	
        s->world[co]->fluxes[p] += -flux;
        s->world[atm]->fluxes[p] += atm_flux;
        s->world[oc]->fluxes[p] += (-sea_spray_P + outflux);

	cout << " wind erosion " << outflux << endl;


        if(DEBUG) {
            cout << "WindErosion::flux::" << flux << endl;
        }

        vector<double> output = {atm_flux, outflux, sea_spray_P};
        this->fluxes.push_back(output);
    }

    bool exec(string param) {
        if (param == "Init") {
            init();
            return true;
        }
        if (param == "Evolve") {
            evolve();
            return true;
        }
        return false;
    }
};
REGISTER_MODULE(WindErosion)
