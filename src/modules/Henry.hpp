class Henry: public Module {
public:
    Henry(string name): Module(name) { init(); }

    double MN2, H0, V0, Mref, Pref, change;
    bool evolution;
    int n2, nhx, atm, oc;

    void init(void) {
        this->links.push_back("Atmosphere0 -> Oceans0");
        this->isBidirectional = true;
        this->numOutputs = 2;
        this->init_fluxes(2);

        evolution = config->data["Henry"]["evolution"].as<bool>();
        change = config->data["Henry"]["change"].as<double>(); // in % (can be +/-)

        H0 = config->data["Henry"]["H0"].as<double>();
        V0 = config->data["Henry"]["V0"].as<double>();
        MN2 = 28e-3; // the masses are in kg
        Pref = 0.8e5;
        Mref = 4e18;

        n2 = s->element_map["n2"];
        nhx = s->element_map["nhx"];

        atm = s->reservoir_map["Atmosphere"];
        oc = s->reservoir_map["Oceans"];
    }

    void evolve(void) {
        double V = V0;
        if (evolution) { V = V0*(1 + change*s->time/4.5e9); }

        double mtot = s->world[atm]->masses[n2] + s->world[oc]->masses[n2];
        double cst = Pref/Mref*H0;
        double m_oc_eq = mtot*cst/(1./V/MN2-cst);
        double flux = (m_oc_eq-s->world[oc]->masses[n2])/s->timestep;

        s->world[atm]->fluxes[n2] += -flux;
        s->world[oc]->fluxes[n2] += flux;

        if(DEBUG) {
            cout << "Henry::flux::" << flux << endl;
            cout << "Henry::V/V0::" << V/V0 << endl;
        }

        vector<double> output = {flux, V/V0};
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
REGISTER_MODULE(Henry)
