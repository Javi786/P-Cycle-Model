import pandas as pd

class Axis:
    def __init__(self, norm, ylabel, earthscale=1):
        self.norm = norm
        self.ylabel = ylabel
        self.earthscale = earthscale

class Parameters:
    # TODO: define a default set of values when an unknown category is accessed
    def __init__(self):
        PARAMS = {}
        PARAMS['default'] = Axis(1, 'undefined axis')

        PARAMS['Atmosphere0'] = Axis(1, 'P content [kg]', earthscale = 2.8e7)
        PARAMS['Oceans0'] = Axis(1, 'P content [kg]', earthscale= 9.3e13)
        #PARAMS['Oceans1'] = Axis(5.1e16, 'NO$_x$ [mM]', earthscale=1e-2)
        #PARAMS['Oceans2'] = Axis(2.4e16, 'NH$_x$ [mM]', earthscale=3e-4)
        #PARAMS['LMantle0'] = Axis(1e20, 'P content [x$10^{20}$ kg]', earthscale= 6)
        PARAMS['LMantle0'] = Axis(1, 'P content [kg]', earthscale= 6e20)
        #PARAMS['UMantle0'] = Axis(1e20, 'P content [x$10^{20}$ kg]', earthscale= 2.2)
        PARAMS['UMantle0'] = Axis(1, 'P content [kg]', earthscale= 2.2e20)
        PARAMS['OCrust0'] = Axis(1, 'P content [kg]', earthscale= 4.5e18)  
        PARAMS['CCrust0'] = Axis(1, 'P content [kg]', earthscale= 1.4e19)    
        PARAMS['Sediments0'] = Axis(1, 'P content [kg]', earthscale= 4e18)
        

        PARAMS['Volcanism0'] = Axis(1, 'Volcanic flux of P [kg $yr^{-1}$]', earthscale=0.373e10)
        PARAMS['Volcanism1'] = Axis(1, 'Volcanic flux of P [kg $yr^{-1}$]', earthscale=1.49e10)
        PARAMS['Volcanism2'] = Axis(1, 'Volcanic flux of P [kg $yr^{-1}$]', earthscale=0.25e10)
        PARAMS['Subduction0'] = Axis(1, 'flux of P [kg $yr^{-1}$]', earthscale=8.22e10)
        PARAMS['Subduction1'] = Axis(1, 'Accretionn flux of P [kg $yr^{-1}$]', earthscale=1.209e10)
        PARAMS['Convection0'] = Axis(1e13, 'flux of P [kg $yr^{-1}$]', earthscale=2.41)
        #PARAMS['Henry0'] = Axis(1, 'Ocean-Atmosphere equilibration flux [kg/yr]', earthscale=1)
        #PARAMS['AbioticFixation0'] = Axis(1, 'Abiotic fixation to NO$_x$ [kg/yr]', earthscale=1)
        #PARAMS['AbioticFixation1'] = Axis(1, 'Abiotic fixation to NH$_x$ [kg/yr]', earthscale=1)
        PARAMS['HydrothermalCirculation0'] = Axis(1e8, 'HT. circulation of P$ [kg $yr^{-1}$]', earthscale=4.4)
        #PARAMS['HydrothermalCirculation1'] = Axis(1e6, 'HT. circulation from N$_2$ [1e6 kg/yr]', earthscale=1)
        #PARAMS['FreundlichAdsorption0'] = Axis(1e10, 'Freundlich adsorption [1e10 kg/yr]', earthscale=1)
        #PARAMS['Erosion0'] = Axis(1, 'Erosion rate [1e10 kg/yr]', earthscale=21.7e10)
        PARAMS['Erosion0'] = Axis(1, 'flux of P [kg $yr^{-1}$]', earthscale=8e10)
        #PARAMS['Erosion1'] = Axis(1, 'Relative continental area', earthscale=0.35)
        PARAMS['WindErosion0'] = Axis(1, 'Erosion rate [kg $yr^{-1}$]', earthscale=2.07e9)
        PARAMS['Precipitation0'] = Axis(1, 'Precipitation Rate [kg $yr^{-1}$]', earthscale=2.8e9)
        PARAMS['CometDelivery0'] = Axis(1, 'ET Input Rate [kg P $yr^{-1}$]', earthscale=2e5)

#        PARAMS['Atmosphere2'] = {'norm':1, 'ylabel':'N-content [kg]', 'isLog':True, 'Earth':1e15}
#        PARAMS['BioticContribution0'] = {'norm':1, 'ylabel':'N-content [kg]', 'isLog':True, 'Earth':1}
#        PARAMS['BioticContribution1'] = {'norm':1, 'ylabel':'Synthesis pathway', 'isLog':False, 'Earth':1}
#        PARAMS['BioticContribution2'] = {'norm':1, 'ylabel':'Assimil. pathway', 'isLog':False, 'Earth':1}
#        PARAMS['CometDelivery0'] = {'norm':1, 'ylabel':'Assimil. pathway', 'isLog':True, 'Earth':1}
#        PARAMS['Impacts0'] = {'norm':1, 'ylabel':'Assimil. pathway', 'isLog':True, 'Earth':1}

        self.params = PARAMS

def loadfile(fname, columns, PARAMS, xaxis='time'):
    if type(columns) != list:
        columns = [columns]

    try:
        data = pd.read_csv(fname)
    except:
        data = pd.DataFrame()

    if not sum([x in data.columns for x in columns]):
        print("File doesn't seem to be an output file, skipping", fname)
        return None, None

    xa = data[xaxis].values[1:]
    if xaxis != 'time':
        xa/= PARAMS[columns[0]].norm

    ya = []
    for col in columns:
        #print(data[col])
        y = data[col].values[1:]
        y /= PARAMS[col].norm
        #print(y)
        ya.append(y)

    print(fname)
    return xa, ya
