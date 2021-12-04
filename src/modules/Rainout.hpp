class Rainout: public Module {
public:


    Rainout(string name): Module(name) { init(); }

    double rainout, satur, flux, relative_area, uniform_growth; //, VAtmos
	//alpha, F0
    int p, atm, oc, co; //From the Atmosphere into Oceans and CCrust

    void init(void) {
        this->links.push_back("Atmosphere -> Oceans2");
        this->links.push_back("Atmosphere -> CCrust2");
        this->isBidirectional = false;
        this->numOutputs = 2;
        this->init_fluxes(2);
        

        satur = config->data["WindErosion"]["satur"].as<double>();
        rainout = config->data["Rainout"]["flux"].as<double>();
        //VAtmos = config->data["Rainout"]["VAtmos"].as<double>();
        uniform_growth = config->data["Rainout"]["uniform_growth"].as<bool>();

        p = s->element_map["p"];

        oc = s->reservoir_map["Oceans"];
        co = s->reservoir_map["CCrust"];
	atm = s->reservoir_map["Atmosphere"];
    }

    void evolve(void) {
        double relative_area;
        if (s->time < 1.5e9) { 
            relative_area = 0.4*(0.02 + 0.64*s->time/1.5e9);
        } else {
            relative_area = 0.4*(0.66 + 0.34*(s->time-1.5e9)/3e9);
        }
        if (uniform_growth) { relative_area = 0.4*(0.02 + 0.98*s->time/4.5e9); } //Parametrization of Emerged land growth
	
	flux = rainout;
	
	if (s->world[atm]->masses[p] < rainout*s->timestep) //If Atm mass is lower than total subtracted mass, ajust to maximum possible removal value
	{
		flux = (s->world[atm]->masses[p])/s->timestep;
	}
        
        //if(s->world[atm]->masses[p] > satur)
        //{
        //	flux = (satur - s->world[atm]->masses[p])/s->timestep;
        //}
	
	//cout <<"flux = " << flux << endl;
        
        double flux_to_oceans = flux*(1-relative_area);
        double flux_to_continents = flux*relative_area;
        
        s->world[oc]->fluxes[p] += flux_to_oceans;
        s->world[co]->fluxes[p] += flux_to_continents;
        s->world[atm]->fluxes[p] += -flux;

	cout << " Rainout flux: " << flux_to_oceans << endl;


        if(DEBUG) {
            cout << "Rainout::flux::" << flux << endl;
        }

        vector<double> output = {flux_to_oceans, flux_to_continents};
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
REGISTER_MODULE(Rainout)


