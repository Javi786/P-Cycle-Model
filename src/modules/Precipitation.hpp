class Precipitation: public Module {
public:

    // Precipitation of P is based on the concentration
    // of P in the ocean and when it is higher than the 
    // concentration of max saturation than the amount
    // of P that is over the maximum saturation will
    // precipitate out of the ocean and accumulate as
    // sediments on the Sediments' reservoir.

    Precipitation(string name): Module(name) { init(); }

    double alpha, F0, precip, max_conc, flux, VOcean;
    int p, sed, oc;

    void init(void) {
        this->links.push_back("Oceans2 -> Sediments");
        this->isBidirectional = false;
        this->numOutputs = 1;
        this->init_fluxes(1);

        max_conc = config->data["Precipitation"]["max_conc"].as<double>(); //Molar Concentration of P on the ocean (mol/liter)
        flux = config->data["Precipitation"]["flux"].as<double>();
        VOcean = config->data["Precipitation"]["VOcean"].as<double>(); //Volume of the Ocean in Liters

        p = s->element_map["p"];

        oc = s->reservoir_map["Oceans"];
        sed = s->reservoir_map["Sediments"];
    }

    void evolve(void) {

        // concentration of P in the ocean for comparison to the 
        // maximum concentration of P. If the value is greater
        // that take the amount of P over the max saturation 
        // that will precipitate out.

        double saturation_mass = max_conc*VOcean*0.031; //Maximum mass allowed on the oceans (saturation mass in kg)

        if (s->world[oc]->masses[p] > saturation_mass)
        {
            precip = s->world[oc]->masses[p] - saturation_mass; 
            // Precip will be a negative value representing 
            // the amount of solute over the maximum concentration
            // that is precipitated out of the oceans. This will be
            // added to the flux as it more volitile than the flux
            flux = precip/s->timestep;

        }

        // add the flux to the world resevoirs within the flux
        // attribute.
        s->world[oc]->fluxes[p] += -flux;
        s->world[sed]->fluxes[p] += flux;

	cout << "Precipitation::flux::" << flux << endl;

        if(DEBUG) {
            cout << "Precipitation::flux::" << flux << endl;
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
REGISTER_MODULE(Precipitation)

