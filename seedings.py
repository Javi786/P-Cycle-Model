from generate_configs import generate_reservoirs, generate_parameters
r1 = {
   'name': 'Atmosphere',
   'element': 'p',
   'log': True,
   'vmin': 1e5,
   'vmax': 1e8
 }
r2 = {
   'name': 'Oceans',
   'element': 'p',
   'log': True,
   'vmin': 1e11,
   'vmax': 1e15
 }
#Specify 7 reservoirs for random initial conditions.
r3 = {
   'name': 'Sediments',
   'element': 'p',
   'log': True,
   'vmin': 1,
   'vmax': 1e2
}
r4 = {
   'name': 'OCrust',
   'element': 'p',
   'log': True,
   'vmin': 1e10,
   'vmax': 8.4e20
}
r5 = {
   'name': 'CCrust',
   'element': 'p',
   'log': True,
   'vmin': 1,
   'vmax': 1e2
}
r6 = {
   'name': 'UMantle',
   'element': 'p',
   'log': True,
   'vmin': 1e18,
   'vmax': 8.4e20
}
r7 = {
   'name': 'LMantle',
   'element': 'p',
   'log': True,
   'vmin': 1e18,
   'vmax': 8.4e20
}
generate_reservoirs([r1, r2, r3, r4, r5, r6, r7], m=8.4e20, n=30, default="./new_config.yaml")
