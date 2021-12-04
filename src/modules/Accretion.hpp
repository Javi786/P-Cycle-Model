class Accretion: public Module {
public:
    Accretion(string name): Module(name) { init(); }

    double alpha, F0, flux;
    int p, co, cr;

    void init(void) {
        this->links.push_back("CCrust2 -> Oceans2");
        this->numOutputs = 1;
        this->init_fluxes(1);

        alpha = config->data["Accretion"]["alpha"].as<double>();
        F0 = config->data["Accretion"]["F_0"].as<double>();
        

        p = s->element_map["p"];

        co = s->reservoir_map["CCrust"];
        cr = s->reservoir_map["OCrust"];
    }

    void evolve(void) {
        // concentration of P in oceanic crust needed to
        // calculate the amount of P moving from oceanic
        // crust to the continential crust.
        double concentration = s->world[co]->masses[p];
        // Accretion flux is the amount of material moved
        // from oceanic to continential crust. This has two 
        // part and the alpha value is the amount of crust
        // moving directly to t
        double flux = F0*(1-alpha)*concentration;

        s->world[co]->fluxes[p] += -flux;
        s->world[cr]->fluxes[p] += flux;

        if(DEBUG) {
            cout << "Accretion::flux::" << flux << endl;
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
REGISTER_MODULE(Accretion)
