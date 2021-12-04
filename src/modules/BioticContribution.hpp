class BioticContribution: public Module {
public:
    BioticContribution(string name): Module(name) { init(); }

    bool const_E6;
    double E6i, MMAX;

    double f3, f4, f5, f6;
    double dbio, onset, E3, E4, E5, E6;
    double EA, ED; 

    void init(void) {
        this->links.push_back("Oceans1 -> Atmosphere0");
        this->links.push_back("Oceans2 -> Atmpsphere0");
        this->links.push_back("Oceans1 -> OCrust2");
        this->links.push_back("Oceans2 -> OCrust2");
        this->numOutputs = 4;
        this->init_fluxes(4);


        // dbio is in kg/year
        const_E6 = config->data["BioticContribution"]["const_E6"].as<bool>();
        dbio = config->data["BioticContribution"]["dbio"].as<double>();
        onset = config->data["BioticContribution"]["onset"].as<double>();
        ED = config->data["BioticContribution"]["ED"].as<double>();
        EA = config->data["BioticContribution"]["EA"].as<double>();
        E6i = config->data["BioticContribution"]["E6i"].as<double>();
        MMAX = config->data["BioticContribution"]["MMAX"].as<double>();

        E6 = E6i;
        E5 = E6;
        E4 = E6*6;
        E3 = E6*2;

        f3 = 1.0;
        f4 = 1.0;
        f5 = 1.0;
        f6 = 1.0;
    }

    void evolve(void) {
        int atm = s->idx_map["Atmosphere"];
        int sed = s->idx_map["OCrust"];
        int oc = s->idx_map["Oceans"];
        int lm = s->idx_map["LMantle"];

        if (s->time < onset) {
            vector<double> output = {0, -1, -1};
            this->fluxes.push_back(output);
            return;
        }

        if (!const_E6) {
            //E6 = E6i * exp(-s->masses[atm+2]/MMAX);
            E6 = E6i * (MMAX-s->masses[atm+2])/MMAX;
            E5 = E6;
            E4 = E6*6;
            E3 = E6*2;
        }

        double bio_increase = dbio*s->timestep;

        // assimilation
        bool assimilated = false;
        double apathway = -1;
        if (not assimilated and s->masses[oc+2] > bio_increase) { // F0
            s->fluxes[oc+2] -= dbio;
            s->fluxes[atm+2] += dbio;
            assimilated = true;
            apathway = 0;
        }
        if (not assimilated and s->masses[oc+1] > bio_increase) { // F1
            s->fluxes[oc+1] -= dbio;
            s->fluxes[oc+2] += dbio;
            assimilated = true;
            apathway = 1;
        }
        if (not assimilated and s->masses[oc] > bio_increase) { // F2
            s->fluxes[oc] -= dbio;
            s->fluxes[oc+2] += dbio;
            assimilated = true;
            apathway = 2;
        }

        // energy synthesis
        bool synthesis = false;
        double flux;
        double spathway = -1;
        double scale = 1; //s->masses[atm+2]/1e16;
        if (not synthesis and ED > 0 and EA > 0) { // F6
            ED -= f6*bio_increase/E6; 
            EA -= f6*bio_increase/E6;
            synthesis = true;
            spathway = 6;
        }
        if (not synthesis and ED <= 0 and EA > 0) { // F3
            flux = f3*bio_increase/E3;
            EA -= flux; 
            if (s->masses[oc+2] < 2*flux) {
                s->fluxes[atm+2] -= s->masses[atm+2]*0.01/s->timestep;
                s->fluxes[oc+2] += s->masses[atm+2]*0.01/s->timestep;
            } else {
                s->fluxes[oc+2] -= scale*flux/s->timestep;
                s->fluxes[oc+1] += scale*flux/s->timestep;
            }
            synthesis = true;
            spathway = 3;
        }
        if (not synthesis and ED > 0 and EA <= 0) { // F4
            flux = f4*bio_increase/E4;
            ED -= flux; 
            if (s->masses[oc+1] < 2*flux) {
                s->fluxes[atm+2] -= s->masses[atm+2]*0.01/s->timestep;
                s->fluxes[oc+2] += s->masses[atm+2]*0.01/s->timestep;
            } else {
                s->fluxes[oc+1] -= scale*flux/s->timestep;
                s->fluxes[oc] += scale*flux/s->timestep;
            }
            synthesis = true;
            spathway = 4;
        }
        if (not synthesis and ED <= 0 and EA <= 0) { // F5
            flux = f5*bio_increase/E5;
            if (s->masses[oc+2] < 2*flux or s->masses[oc+1] < 2*flux) {
                s->fluxes[atm+2] -= s->masses[atm+2]*0.01/s->timestep;
                s->fluxes[oc+2] += s->masses[atm+2]*0.01/s->timestep;
            } else {
                s->fluxes[oc+1] -= 0.5*scale*flux/s->timestep; 
                s->fluxes[oc+2] -= 0.5*scale*flux/s->timestep;
                s->fluxes[oc] += scale*flux/s->timestep;
            }
            synthesis = true;
            spathway = 5;
        }

        if(DEBUG) {
            cout << "BioticContribution::BS::" << s->masses[atm+2] << endl;
        }

        vector<double> output = {s->masses[atm+2], spathway, apathway, flux/s->timestep};
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
REGISTER_MODULE(BioticContribution)
