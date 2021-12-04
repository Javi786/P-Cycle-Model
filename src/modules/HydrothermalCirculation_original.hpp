class HydrothermalCirculation: public Module {
public:
    HydrothermalCirculation(string name): Module(name) { init(); }

    double tau, F_NOx, F_N2, F0, F1;
    int n2, nox, nhx, cr, oc;

    void init(void) {
        this->links.push_back("Oceans0 -> Oceans2");
        this->links.push_back("Oceans1 -> Oceans2");
        this->numOutputs = 2;
        this->init_fluxes(2);

        F0 = config->data["HydrothermalCirculation"]["F0"].as<double>();
        F1 = config->data["HydrothermalCirculation"]["F1"].as<double>();
        F_NOx = config->data["HydrothermalCirculation"]["F_NOx"].as<double>();
        F_N2 = config->data["HydrothermalCirculation"]["F_N2"].as<double>();
        tau = config->data["HydrothermalCirculation"]["tau"].as<double>();

        n2 = s->element_map["n2"];
        nox = s->element_map["nox"];
        nhx = s->element_map["nhx"];

        cr = s->reservoir_map["OCrust"];
        oc = s->reservoir_map["Oceans"];
    }

    void evolve(void) {
        double vol_fraction = F0 + (F1-F0)*exp(-s->time/tau);
        double f_NOx = s->world[oc]->masses[nox]*vol_fraction*F_NOx;
        double f_N2 = s->world[oc]->masses[n2]*vol_fraction*F_N2;

        s->world[oc]->fluxes[n2] += -f_N2;
        s->world[oc]->fluxes[nox] += -f_NOx;
        s->world[oc]->fluxes[nhx] += (f_N2+f_NOx);

        if(DEBUG) {
            cout << "HydrothermalCirculation::f_NOx::" << f_NOx << endl;
            cout << "HydrothermalCirculation::f_N2::" << f_N2 << endl;
        }

        vector<double> output = {f_NOx, f_N2};
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
REGISTER_MODULE(HydrothermalCirculation)
