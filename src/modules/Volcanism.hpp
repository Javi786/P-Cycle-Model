class Volcanism: public Module {
public:
    Volcanism(string name): Module(name) { init(); }

    double F_arc, F_MORB, F_hotspot, Factor, tau, P_Conc_Ratio_MORB, P_Conc_Ratio_ARC, P_Conc_Ratio_HS;
    int p, co, cr, um, lm;
    bool uniform_growth;
    

    void init(void) {
        this->links.push_back("LMantle2 -> CCrust2"); // Hotspot to Continent
        this->links.push_back("LMantle2 -> OCrust2"); // Hotspot to Oceanic Crust
        this->links.push_back("UMantle2 -> OCrust2"); // Mid-Ocean Ridge Volcanism (To Oceanic Crust)
        this->links.push_back("UMantle2 -> CCrust2"); // Arc Volcanism (To Continents)

        this->numOutputs = 3;
        this->init_fluxes(4);

        // these are given in km3/yr
        F_arc = config->data["Volcanism"]["F_arc"].as<double>();
        F_MORB = config->data["Volcanism"]["F_MORB"].as<double>();
        F_hotspot = config->data["Volcanism"]["F_hotspot"].as<double>();
        
        Factor = config->data["Volcanism"]["Factor"].as<double>();
        tau = config->data["Volcanism"]["tau"].as<double>();
        P_Conc_Ratio_MORB = config->data["Volcanism"]["P_Conc_Ratio_MORB"].as<double>();
        P_Conc_Ratio_ARC = config->data["Volcanism"]["P_Conc_Ratio_ARC"].as<double>();
        P_Conc_Ratio_HS = config->data["Volcanism"]["P_Conc_Ratio_HS"].as<double>();
        uniform_growth = config->data["Erosion"]["uniform_growth"].as<bool>();
        
        // oxidizing = 0.2 corresponds to partitioning in Jim's notes
        //oxidizing = config->data["man_ox"].as<double>();
        //density = config->data["Volcanism"]["density"].as<double>();
        //scaling = config->data["Volcanism"]["scaling"].as<double>();

        p = s->element_map["p"];

        um = s->reservoir_map["UMantle"];
        lm = s->reservoir_map["LMantle"];
        cr = s->reservoir_map["OCrust"];
        co = s->reservoir_map["CCrust"];
    }

    void evolve(void) {
        //double factor_um = s->world[um]->masses[p]/scaling;      //From Marcos' previous work
        //double factor_lm = s->world[lm]->masses[p]/scaling;
        //double flux_arc = oxidizing*(F_MORB+F_arc)*density*1e9*factor_um;
        //double flux_MORB = (1-oxidizing)*(F_MORB+F_arc)*density*1e9*factor_um;
        //double flux_hotspot = F_hotspot*density*1e9*factor_lm;
        
        double vol_factor = 1 + (Factor - 1)*exp(-s->time/tau);

	double lm_volume = 5.9e11; //Volume of the Lower Mantle in km³
	double um_volume = 2.95e11; //Volume of the Upper Mantle in km³
	double lm_concentration = s->world[lm]->masses[p]/lm_volume;  //mass concentration of P in the Lower Mantle in kg/km³
	double um_concentration = s->world[um]->masses[p]/um_volume;  //mass concentration of P in the Upper Mantle in kg/km³

	double flux_arc = vol_factor*F_arc*um_concentration*P_Conc_Ratio_ARC;          //mass flux of P in kg for Arc volcanism
	double flux_MORB = vol_factor*F_MORB*um_concentration*P_Conc_Ratio_MORB;       //mass flux of P in kg for MORB volcanism
	double flux_hotspot = vol_factor*F_hotspot*lm_concentration*P_Conc_Ratio_HS;   //mass flux of P in kg for Hotspot volcanism

	double relative_area;
        if (s->time < 1.5e9) { 
            relative_area = 0.4*(0.02 + 0.64*s->time/1.5e9);
        } else {
            relative_area = 0.4*(0.66 + 0.34*(s->time-1.5e9)/3e9);
        }
        if (uniform_growth) { relative_area = 0.4 - 2.613e-3*exp(- s->time/0.15e9);  }


        s->world[um]->fluxes[p] -= (flux_arc + flux_MORB);
        s->world[lm]->fluxes[p] -= flux_hotspot;
        //s->world[atm]->fluxes[p] += flux_arc;
        // s->fluxes[atm+1] += (flux_MORB + flux_hotspot); this goes directly
        // back to the oceans in practice
        //s->world[oc]->fluxes[p] += (flux_MORB + flux_hotspot);
        s->world[cr]->fluxes[p] += (flux_MORB + (1-relative_area)*flux_hotspot); // into Oceanic Crust
        s->world[co]->fluxes[p] += (flux_arc + relative_area*flux_hotspot);      // into Continental Crust
        

        if(DEBUG) {
            cout << "Volcanism::flux_arc::" << flux_arc << endl;
            cout << "Volcanism::flux_MORB::" << flux_MORB << endl;
            cout << "Volcanism::flux_hotspot::" << flux_hotspot << endl;
        }

        vector<double> output = {flux_arc, flux_MORB, flux_hotspot};
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
REGISTER_MODULE(Volcanism)
