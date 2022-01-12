class Erosion: public Module {
public:
    Erosion(string name): Module(name) { init(); }

    double alpha, A0, M0, rho, max_conc, VOcean;
    bool uniform_growth;
    int p, co, oc, sed;

    void init(void) {
        this->links.push_back("CCrust2 -> Oceans2");
        this->links.push_back("CCrust2 -> Sediments");
        this->isBidirectional = true;
        this->numOutputs = 1;
        this->init_fluxes(2);

        alpha = config->data["Erosion"]["alpha"].as<double>();
        uniform_growth = config->data["Erosion"]["uniform_growth"].as<bool>();
        max_conc = config->data["Precipitation"]["max_conc"].as<double>();
        VOcean = config->data["Precipitation"]["VOcean"].as<double>(); //Volume of the Ocean in Liters
        
        A0 = 1.48e14; // m2, today
        M0 = 2.28e22; // kg, today
        rho = 2700; // kg/m3

        p = s->element_map["p"];

        co = s->reservoir_map["CCrust"];
        oc = s->reservoir_map["Oceans"];
        sed = s->reservoir_map["Sediments"];
    }

    void evolve(void) {
        double relative_area;
        if (s->time < 1.5e9) { 
            relative_area = 0.4*(0.02 + 0.64*s->time/1.5e9);
        } else {
            relative_area = 0.4*(0.66 + 0.34*(s->time-1.5e9)/3e9);
        }
        if (uniform_growth) { relative_area = 0.4 - 2.613e-3*exp(- s->time/0.15e9); }

        double concentration = s->world[co]->masses[p]/(M0); // wt% of P in the continental crust = 0.1% -> THIS
        //double concentration = 0.001; //Other option? is it better?
        double flux = alpha*A0*rho*relative_area*concentration;   //           kg of P per year. Present day = 5e-4 x 1.48e14 x 0.4 x 2700 x 0.001 = 8e10 kg of P /yr
	double outflux = 0;
	double oce_flux = flux;
	
	double max_mass = max_conc*VOcean*0.031;

	if (s->world[oc]->masses[p] + flux*s->timestep > max_mass && s->world[oc]->masses[p] <= max_mass)
	{
		outflux = (s->world[oc]->masses[p] + flux*s->timestep - max_mass)/s->timestep;
		
		oce_flux = flux - outflux;
		
		//if (oce_flux < 0)
		//{
		//	oce_flux = 0;
		//	outflux = flux;
		//}
		
	}
	
	if (s->world[oc]->masses[p] + flux*s->timestep > max_mass && s->world[oc]->masses[p] > max_mass)
	{
		oce_flux = 0;
		outflux = flux;
	}

	cout << "Continent mass" << s->world[oc]->masses[p] <<"riverine flux: " << flux << " into_ocean " << oce_flux << " out " << outflux << endl;

        s->world[co]->fluxes[p] += -flux;
        s->world[oc]->fluxes[p] += oce_flux;
        s->world[sed]->fluxes[p] += outflux;

        if(DEBUG) {
            cout << "Erosion::flux::" << flux << endl;
            cout << "Erosion::relative_area::" << relative_area << endl;
        }

        vector<double> output = {flux};
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
REGISTER_MODULE(Erosion)
