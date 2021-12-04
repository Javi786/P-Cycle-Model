class FreundlichAdsorption: public Module {
public:
    FreundlichAdsorption(string name): Module(name) { init(); }

    double Kf, VSed, VOceans, rhoSed, change;
    int nhx, cr, oc;
    bool evolution;

    void init(void) {
        this->links.push_back("Oceans2 -> OCrust2");
        this->isBidirectional = true;
        this->numOutputs = 1;
        this->init_fluxes(1);

        evolution = config->data["Henry"]["evolution"].as<bool>();
        change = config->data["Henry"]["change"].as<double>(); // in % (can be +/-)

        Kf = config->data["FreundlichAdsorption"]["Kf"].as<double>();
        VSed = config->data["FreundlichAdsorption"]["VSed"].as<double>();
        rhoSed = config->data["FreundlichAdsorption"]["rhoSed"].as<double>();
        VOceans = config->data["Henry"]["V0"].as<double>();

        nhx = s->element_map["nhx"];

        cr = s->reservoir_map["OCrust"];
        oc = s->reservoir_map["Oceans"];
    }

    void evolve(void) {
        double V = VOceans;
        if (evolution) { V = VOceans*(1 + change*s->time/4.5e9); }

        double mtot = s->world[oc]->masses[nhx] + s->world[cr]->masses[nhx];
        double cst = VSed/V*rhoSed*Kf;
        double m_oc_eq = mtot/(1+cst);
        double flux = -(m_oc_eq-s->world[oc]->masses[nhx])/s->timestep;

        s->world[cr]->fluxes[nhx] += flux;
        s->world[oc]->fluxes[nhx] += -flux;

        if(DEBUG) cout << "FreundlichAdsorption::flux::" << flux << endl;

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
REGISTER_MODULE(FreundlichAdsorption)
