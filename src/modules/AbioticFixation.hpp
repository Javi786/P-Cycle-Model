class AbioticFixation: public Module {
public:
    AbioticFixation(string name): Module(name) { init(); }

    double Fi_NOx, Ff_NOx, Fi_NH3, Ff_NH3, M_REF, tau, goe;
    int n2, nox, nhx, atm, oc;

    void init(void) {
        this->links.push_back("Atmosphere0 -> Oceans1");
        this->links.push_back("Atmosphere0 -> Oceans2");
        this->numOutputs = 2;
        this->init_fluxes(2);

        Fi_NOx = config->data["AbioticFixation"]["Fi_NOx"].as<double>();
        Fi_NH3 = config->data["AbioticFixation"]["Fi_NH3"].as<double>();
        Ff_NOx = config->data["AbioticFixation"]["Ff_NOx"].as<double>();
        Ff_NH3 = config->data["AbioticFixation"]["Ff_NH3"].as<double>();
        tau = config->data["AbioticFixation"]["tau"].as<double>();
        goe = config->data["AbioticFixation"]["goe"].as<double>();
        M_REF = config->data["AbioticFixation"]["M_REF"].as<double>();

        n2 = s->element_map["n2"];
        nox = s->element_map["nox"];
        nhx = s->element_map["nhx"];

        atm = s->reservoir_map["Atmosphere"];
        oc = s->reservoir_map["Oceans"];
    }

    void evolve(void) {
        double F_NOx = Ff_NOx + Fi_NOx*(1-1/(1+exp(-(s->time-goe)/tau)));
        double F_NH3 = Ff_NH3 + Fi_NH3*(1-1/(1+exp(-(s->time-goe)/tau)));

        double flux_NOx = F_NOx*s->world[atm]->masses[n2]/M_REF;
        double flux_NH3 = F_NH3*s->world[atm]->masses[n2]/M_REF;

        s->world[atm]->fluxes[n2] += -(flux_NOx + flux_NH3);
        s->world[oc]->fluxes[nox] += flux_NOx;
        s->world[oc]->fluxes[nhx] += flux_NH3;

        if(DEBUG) {
            cout << "AbioticFixation::flux_NOx::" << flux_NOx << endl;
            cout << "AbioticFixation::flux_NH3::" << flux_NH3 << endl;
        }

        vector<double> output = {flux_NOx, flux_NH3};
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
REGISTER_MODULE(AbioticFixation)
