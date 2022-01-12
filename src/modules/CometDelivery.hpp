class CometDelivery: public Module {
public:
    CometDelivery(string name): Module(name) { init(); }

    double p, tau, F0, F1, max_conc, VOcean; //this
    bool uniform_growth;

    void init(void) {
        this->links.push_back("Space -> Oceans2");
        this->links.push_back("Space -> Sediments"); //this
        this->links.push_back("Space -> CCrust2");
        this->numOutputs = 1;
        this->init_fluxes(1);

        F0 = config->data["CometDelivery"]["F0"].as<double>();
        F1 = config->data["CometDelivery"]["F1"].as<double>();
        tau = config->data["CometDelivery"]["tau"].as<double>();
        max_conc = config->data["Precipitation"]["max_conc"].as<double>(); //this
        VOcean = config->data["Precipitation"]["VOcean"].as<double>(); //Volume of the Ocean in Liters 
        uniform_growth = config->data["Erosion"]["uniform_growth"].as<bool>();
    }

    void evolve(void) {
        int oc = s->reservoir_map["Oceans"];
        int sed = s->reservoir_map["Sediments"];   //this
        int co = s->reservoir_map["CCrust"];
        
        
        double relative_area;
        if (s->time < 1.5e9) { 
            relative_area = 0.4*(0.02 + 0.64*s->time/1.5e9);
        } else {
            relative_area = 0.4*(0.66 + 0.34*(s->time-1.5e9)/3e9);
        }
        if (uniform_growth) { relative_area = 0.4 - 2.613e-3*exp(- s->time/0.15e9); 
        }
        
        double max_mass = max_conc*VOcean*0.031;

        double flux = F0 + (F1-F0)*exp(-s->time/tau);
        
        double total_flux = F0 + (F1-F0)*exp(-s->time/tau);
        
        double Cont_flux = flux*relative_area;
        
        flux = flux*(1-relative_area);
        
        double outflux = 0; //this
        double oce_flux = flux; //this
        
        if(s->world[oc]->masses[p] + flux*s->timestep > max_mass && s->world[oc]->masses[p] <= max_mass) //all this
        {
        	outflux = (s->world[oc]->masses[p] + flux*s->timestep - max_mass)/s->timestep;
		
		oce_flux = flux - outflux;
        }
        
        if (s->world[oc]->masses[p] + flux*s->timestep > max_mass && s->world[oc]->masses[p] > max_mass) //all this
	{
		oce_flux = 0;
		outflux = flux;
	}

        s->world[oc]->fluxes[p] += oce_flux; // this is a net source, so no need to balance //this
        s->world[sed]->fluxes[p] += outflux; //this
        s->world[co]->fluxes[p] += Cont_flux; //this

	cout << "CometDelivery to ocean flux:: " << oce_flux << " to sediments flux " << outflux << endl;
	
        if(DEBUG) cout << "CometDelivery::flux::" << flux << endl;

        vector<double> output = {total_flux};
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
REGISTER_MODULE(CometDelivery)
