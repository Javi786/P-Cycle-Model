class Subduction: public Module {
public:
    Subduction(string name): Module(name) { init(); }

    double tau, accretion; // VCrust
    int p, cr, co, um, sed;

    void init(void) {
        this->links.push_back("OCrust2 -> UMantle2");
        this->links.push_back("Sediments -> CCrust2");  
        this->links.push_back("Sediments -> UMantle2");     //Perhaps to be included, some interchange btwn sediments and ocean crust
        this->numOutputs = 2;
        this->init_fluxes(2);

        //VCrust = config->data["Subduction"]["VCrust"].as<double>();
        tau = config->data["Subduction"]["tau"].as<double>();
        accretion = config->data["Subduction"]["accretion"].as<double>();  //accretion fraction: fraction of sediments that accrete into CCrust at subduction zones

        p = s->element_map["p"];

        cr = s->reservoir_map["OCrust"];
        co = s->reservoir_map["CCrust"];
        um = s->reservoir_map["UMantle"];
        sed = s->reservoir_map["Sediments"];
    }

    void evolve(void) {
        //double flux = s->masses[cr+2]*s->timestep/tau;
        double flux = s->world[cr]->masses[p]/tau;
        double flux_acc = s->world[sed]->masses[p]/tau;
        s->world[cr]->fluxes[p] += -flux;
        s->world[sed]->fluxes[p] += -flux_acc;
        //s->world[um]->fluxes[p] += (1-accretion)*flux;
        //s->world[co]->fluxes[p] += accretion*flux;
        s->world[um]->fluxes[p] += (flux + (1-accretion)*flux_acc);
        s->world[co]->fluxes[p] += accretion*flux_acc;

        if(DEBUG) cout << "Subduction::flux::" << flux << endl;

        //vector<double> output = {(1-accretion)*flux, accretion*flux};
        vector<double> output = {(flux + (1-accretion)*flux_acc), accretion*flux_acc};
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
REGISTER_MODULE(Subduction)
